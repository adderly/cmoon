#include "mheads.h"

NEOERR* mmg_init(char *host, int port, int ms, mmg_conn **db)
{
    if (!host) return nerr_raise(NERR_ASSERT, "paramter null");

    mmg_conn *ldb;

    mtc_noise("connect to %s %d %d ...", host, port, ms);
    
    *db = NULL;
    ldb = calloc(1, sizeof(mmg_conn));
    if (!ldb) return nerr_raise(NERR_NOMEM, "calloc for connection");

    ldb->con = mongo_sync_connect(host, port, true);
    if (!ldb->con) return nerr_raise(NERR_DB, "sync connect: %s", strerror(errno));
    mongo_sync_conn_set_auto_reconnect(ldb->con, true);
    if (ms > 0) mongo_connection_set_timeout((mongo_connection*)ldb->con, ms);

    *db = ldb;

    return STATUS_OK;
}

NEOERR* mmg_auth(mmg_conn *db, char *dsn, char *user, char *pass)
{
    mtc_noise("authorize to %s use %s %s", dsn, user, pass);
    
    if (!mongo_sync_cmd_authenticate(db->con, dsn, user, pass))
        return nerr_raise(NERR_DB, "auth: %s", strerror(errno));

    return STATUS_OK;
}

NEOERR* mmg_seed_add(mmg_conn *db, char *host, int port)
{
    mtc_noise("add seed %s %d ...", host, port);

    if (!mongo_sync_conn_seed_add(db->con, host, port))
        return nerr_raise(NERR_DB, "add seed: %s", strerror(errno));
    
    return STATUS_OK;
}

void mmg_destroy(mmg_conn *db)
{
    if (!db) return;

    if (db->c) {
        mongo_sync_cursor_free(db->c);
        db->c = NULL;
    }
    /* TODO packet */
    if (db->docs) {
        bson_free(db->docs);
        db->docs = NULL;
    }
    if (db->docq) {
        bson_free(db->docq);
        db->docq = NULL;
    }

    mongo_sync_disconnect(db->con);
}

NEOERR* mmg_prepare(mmg_conn *db, int flags, int skip, int limit,
                    NEOERR* (*qcbk)(mmg_conn *db, HDF *node),
                    char *sels, char *querys)
{
    if (!db || !querys) return nerr_raise(NERR_ASSERT, "paramter null");

    mtc_noise("prepare %s %s", querys, sels);
    
    /*
     * doc query
     */
    if (db->docq) {
        bson_free(db->docq);
        db->docq = NULL;
    }
    db->docq = mbson_new_from_string(querys, true);
    if (!db->docq) return nerr_raise(NERR_ASSERT, "build query: %s: %s",
                                     querys, strerror(errno));


    /*
     * doc selector
     */
    if (db->docs) {
        bson_free(db->docs);
        db->docs = NULL;
    }
    if (sels) {
        db->docs = mbson_new_from_string(sels, false);
        if (!db->docs) return nerr_raise(NERR_ASSERT, "build selector: %s: %s",
                                         sels, strerror(errno));
        if (!(flags & MMG_FLAG_GETOID)) bson_append_int32(db->docs, "_id", 0);
        bson_finish(db->docs);
    } else if (!(flags & MMG_FLAG_GETOID)) {
        db->docs = bson_build(BSON_TYPE_INT32, "_id", 0, BSON_TYPE_NONE);
        bson_finish(db->docs);
    }

    /*
     * later mmg_prepare won't overwrite formal's callback
     */
    if (db->incallback && qcbk != NULL)
        return nerr_raise(NERR_ASSERT, "already in callback, can't set callback");
    if (!db->incallback) db->query_callback = qcbk;
    db->flags = flags;
    db->skip  = skip;
    db->limit = limit;
    db->rescount = 0;
    
    return STATUS_OK;
}

NEOERR* mmg_preparef(mmg_conn *db, int flags, int skip, int limit,
                     NEOERR* (*qcbk)(mmg_conn *db, HDF *node), char *sels, char *qfmt, ...)
{
    char *querys;
    va_list ap;
    NEOERR *err;

    va_start(ap, qfmt);
    querys = vsprintf_alloc(qfmt, ap);
    va_end(ap);
    if (!querys) return nerr_raise(NERR_NOMEM, "Unable to allocate mem for query string");

    err = mmg_prepare(db, flags, skip, limit, qcbk, sels, querys);
    if (err != STATUS_OK) return nerr_pass(err);

    free(querys);
    return STATUS_OK;
}

void mmg_set_callbackdata(mmg_conn *db, void *data)
{
    if (!db || !data) return;

    db->callbackdata = data;
}

NEOERR* mmg_query(mmg_conn *db, char *dsn, char *prefix, HDF *outnode)
{
    int count;
    char key[LEN_HDF_KEY];
    HDF *node, *cnode;
    bson *doc;
    NEOERR *err;
    
    MCS_NOT_NULLB(db, dsn);

    db->p = mongo_sync_cmd_query(db->con, dsn, db->flags & 0x3FF, db->skip, db->limit,
                                 db->docq, db->docs);
    if (!db->p) {
        if (errno == ENOENT) {
            mtc_noise("queried %s 0 result", dsn);
            if (db->flags & MMG_FLAG_EMPTY) return STATUS_OK;
            return nerr_raise(NERR_NOT_FOUND, "document not found");
        }
        return nerr_raise(NERR_DB, "query: %s %d", strerror(errno), errno);
    }

    /*
     * process result
     */
    if (outnode || (db->query_callback && !db->incallback)) {
        if (outnode) node = outnode; /* need store result */
        else hdf_init(&node);

        db->c = mongo_sync_cursor_new(db->con, dsn, db->p);
        if (!db->c) return nerr_raise(NERR_DB, "cursor: %s", strerror(errno));

        cnode = NULL;
        count = 0;
        while (mongo_sync_cursor_next(db->c) && count < db->limit) {
            memset(key, sizeof(key), 0x0);
            
            if (prefix) {
                if (!(db->flags & MMG_FLAG_MIXROWS) && db->limit > 1)
                    snprintf(key, sizeof(key), "%s.%d", prefix, count);
                else snprintf(key, sizeof(key), "%s", prefix);
            } else {
                if (!(db->flags & MMG_FLAG_MIXROWS) && db->limit > 1)
                    sprintf(key, "%d", count);
                else key[0] = '\0';
            }
            
            doc = mongo_sync_cursor_get_data(db->c);
            err = mbson_export_to_hdf(node, doc, key, MBSON_EXPORT_TYPE, true);
            if (err != STATUS_OK) return nerr_pass(err);

            if (!cnode) cnode = hdf_get_obj(node, key);
            count++;
        }
        db->rescount = count;

        mongo_sync_cursor_free(db->c);
        db->c = NULL;
        db->p = NULL;
        
        mtc_noise("queried %s %d result", dsn, count);

        /*
         * call callback at last. because we don't want declare more mmg_conn*
         * it's safe to do new query in callback on result stored (db->c freeed)
         * we can call mmg_query() recursive, the callback won't.
         */
        if (db->query_callback && !db->incallback) {
            db->incallback = true;
            
            while (cnode) {
                err = db->query_callback(db, cnode);
                TRACE_NOK(err);
                
                cnode = hdf_obj_next(cnode);
            }
            
            db->incallback = false;

            /*
             * query_callback can't be shared with multiply query
             * later query must set them again even if TheSameOne
             */
            db->query_callback = NULL;
            db->callbackdata = NULL;
        }

        if (!outnode) hdf_destroy(&node);
    } else {
        /* don't need result */
        mongo_wire_packet_free(db->p);
        db->c = NULL;
        db->p = NULL;
    }

    return STATUS_OK;
}

NEOERR* mmg_string_insert(mmg_conn *db, char *dsn, char *str)
{
    bson *doc;
    
    MCS_NOT_NULLC(db, dsn, str);

    mtc_noise("insert string %s %s", dsn, str);
    
    doc = mbson_new_from_string(str, true);
    if (!doc) return nerr_raise(NERR_ASSERT, "build doc: %s: %s",
                                str, strerror(errno));

    if (!mongo_sync_cmd_insert(db->con, dsn, doc, NULL))
        return nerr_raise(NERR_DB, "sync_cmd_insert: %s", strerror(errno));

    bson_free(doc);

    return STATUS_OK;
}

NEOERR* mmg_string_insertf(mmg_conn *db, char *dsn, char *fmt, ...)
{
    char *qa;
    va_list ap;
    NEOERR *err;
    
    va_start(ap, fmt);
    qa = vsprintf_alloc(fmt, ap);
    va_end(ap);
    if (!qa) return nerr_raise(NERR_NOMEM, "Unable to allocate mem for string");

    err = mmg_string_insert(db, dsn, qa);
    if (err != STATUS_OK) return nerr_pass(err);

    free(qa);

    return STATUS_OK;
}

NEOERR* mmg_hdf_insert(mmg_conn *db, char *dsn, HDF *node)
{
    bson *doc;
    NEOERR *err;
    
    MCS_NOT_NULLC(db, dsn, node);

    mtc_noise("insert hdf %s", dsn);
    
    err = mbson_import_from_hdf(node, &doc, true);
    if (err != STATUS_OK) return nerr_pass(err);

    if (!mongo_sync_cmd_insert(db->con, dsn, doc, NULL))
        return nerr_raise(NERR_DB, "sync_cmd_insert: %s", strerror(errno));

    bson_free(doc);

    return STATUS_OK;
}

NEOERR* mmg_string_update(mmg_conn *db, char *dsn, char *up, char *sel)
{
    bson *doca, *docb;

    MCS_NOT_NULLC(db, dsn, up);

    mtc_noise("update %s %s %s", dsn, up, sel);
    
    if (sel) {
        doca = mbson_new_from_string(sel, true);
        if (!doca) return nerr_raise(NERR_ASSERT, "build doc sel: %s: %s",
                                     sel, strerror(errno));
    } else doca = NULL;
    
    docb = mbson_new_from_string(up, true);
    if (!docb) return nerr_raise(NERR_ASSERT, "build doc up: %s: %s",
                                 up, strerror(errno));

/*
    if (!mongo_sync_cmd_update(db->con, dsn, 0, doca, docb))
*/
    if (!mongo_sync_cmd_update(db->con, dsn,
                               MONGO_WIRE_FLAG_UPDATE_UPSERT,
                               doca, docb))
        return nerr_raise(NERR_DB, "sync_cmd_update: %s", strerror(errno));
    
    bson_free(doca);
    bson_free(docb);
    
    return STATUS_OK;
}

NEOERR* mmg_string_updatef(mmg_conn *db, char *dsn, char *up, char *selfmt, ...)
{
    char *qa;
    va_list ap;
    NEOERR *err;
    
    va_start(ap, selfmt);
    qa = vsprintf_alloc(selfmt, ap);
    va_end(ap);
    if (!qa) return nerr_raise(NERR_NOMEM, "Unable to allocate mem for string");

    err = mmg_string_update(db, dsn, up, qa);
    if (err != STATUS_OK) return nerr_pass(err);

    free(qa);

    return STATUS_OK;
}

NEOERR* mmg_count(mmg_conn *db, char *dbname, char *collname, int *ret, char *querys)
{
    bson *doc;

    MCS_NOT_NULLC(db, dbname, collname);
    MCS_NOT_NULLB(querys, ret);

    mtc_noise("count %s.%s %s", dbname, collname, querys);
    
    doc = mbson_new_from_string(querys, true);
    if (!doc) return nerr_raise(NERR_ASSERT, "build doc: %s: %s",
                                querys, strerror(errno));

    *ret = (int)mongo_sync_cmd_count(db->con, dbname, collname, doc);

    bson_free(doc);

    return STATUS_OK;
}

NEOERR* mmg_countf(mmg_conn *db, char *dbname, char *collname, int *ret, char *qfmt, ...)
{
    char *qa;
    va_list ap;
    NEOERR *err;
    
    va_start(ap, qfmt);
    qa = vsprintf_alloc(qfmt, ap);
    va_end(ap);
    if (!qa) return nerr_raise(NERR_NOMEM, "Unable to allocate mem for string");

    err = mmg_count(db, dbname, collname, ret, qa);
    if (err != STATUS_OK) return nerr_pass(err);

    free(qa);

    return STATUS_OK;
}

NEOERR* mmg_delete(mmg_conn *db, char *dsn, int flags, char *sel)
{
    bson *doc;

    MCS_NOT_NULLC(db, dsn, sel);

    mtc_noise("delete %s %s", dsn, sel);

    doc = mbson_new_from_string(sel, true);
    if (!doc) return nerr_raise(NERR_ASSERT, "build doc: %s: %s",
                                sel, strerror(errno));

    if (!mongo_sync_cmd_delete(db->con, dsn, flags, doc))
        return nerr_raise(NERR_DB, "sync_cmd_delete: %s", strerror(errno));

    bson_free(doc);

    return STATUS_OK;
}

NEOERR* mmg_deletef(mmg_conn *db, char *dsn, int flags, char *selfmt, ...)
{
    char *qa;
    va_list ap;
    NEOERR *err;
    
    va_start(ap, selfmt);
    qa = vsprintf_alloc(selfmt, ap);
    va_end(ap);
    if (!qa) return nerr_raise(NERR_NOMEM, "Unable to allocate mem for string");

    err = mmg_delete(db, dsn, flags, qa);
    if (err != STATUS_OK) return nerr_pass(err);

    free(qa);

    return STATUS_OK;
}

NEOERR* mmg_custom(mmg_conn *db, char *dbname,
                   char *prefix, HDF *outnode, char *command)
{
    mongo_packet *p;
    mongo_sync_cursor *c;
    bson *doc;

    NEOERR *err;
    
    MCS_NOT_NULLB(db, dbname);
    MCS_NOT_NULLB(outnode, command);

    mtc_noise("custom %s %s", dbname, command);

    /*
     * doc
     */
    doc = mbson_new_from_string(command, true);
    if (!doc) return nerr_raise(NERR_ASSERT, "build doc custom %s", command);

    p = mongo_sync_cmd_custom(db->con, dbname, doc);
    bson_free(doc);
    if (!p)
        return nerr_raise(NERR_DB, "sync_cmd_custom: %s %s", command, strerror(errno));

    c = mongo_sync_cursor_new(db->con, dbname, p);
    if (!c) return nerr_raise(NERR_DB, "cursor: %s", strerror(errno));

    if (mongo_sync_cursor_next(c)) {
        doc = mongo_sync_cursor_get_data(c);
        if (!doc) return nerr_raise(NERR_DB, "doc: %s", strerror(errno));
        
        err = mbson_export_to_hdf(outnode, doc, prefix, MBSON_EXPORT_TYPE, true);
        if (err != STATUS_OK) return nerr_pass(err);
    } else return nerr_raise(NERR_DB, "cursor next: %s", strerror(errno));
    
    mongo_sync_cursor_free(c);

    return STATUS_OK;
}

NEOERR* mmg_customf(mmg_conn *db, char *dbname,
                    char *prefix, HDF *outnode, char *cmdfmt, ...)
{
    char *qa;
    va_list ap;
    NEOERR *err;
    
    va_start(ap, cmdfmt);
    qa = vsprintf_alloc(cmdfmt, ap);
    va_end(ap);
    if (!qa) return nerr_raise(NERR_NOMEM, "Unable to allocate mem for string");

    err = mmg_custom(db, dbname, prefix, outnode, qa);
    if (err != STATUS_OK) return nerr_pass(err);

    free(qa);

    return STATUS_OK;
}
