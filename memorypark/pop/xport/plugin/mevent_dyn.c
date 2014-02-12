#include "mevent_plugin.h"
#include "mevent_dyn.h"

#define PLUGIN_NAME    "dyn"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

struct dyn_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct dyn_entry {
    struct event_entry base;
    mdb_conn *db;
    struct cache *cd;
    struct dyn_stats st;
};

#define TRACK_COL " id, type, aid, aname, uid, uname, oid, oname, ip, url, title, refer "

static NEOERR* dyn_cmd_addtrack(struct queue_entry *q, struct cache *cd, mdb_conn *db)
{
    STRING str; string_init(&str);
    char *aname, *uname, *oname;
    int aid, uid, oid = 0;
    NEOERR *err;

    REQ_GET_PARAM_STR(q->hdfrcv, "uname", uname);
    REQ_GET_PARAM_STR(q->hdfrcv, "aname", aname);
    REQ_FETCH_PARAM_STR(q->hdfrcv, "oname", oname);
    uid = hash_string(uname);
    aid = hash_string(aname);
    if (oname) oid = hash_string(oname);

    hdf_set_int_value(q->hdfrcv, "aid", aid);
    hdf_set_int_value(q->hdfrcv, "uid", uid);
    hdf_set_int_value(q->hdfrcv, "oid", oid);
    
    err = mcs_build_incol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.track"),
                          &str);
    if (err != STATUS_OK) return nerr_pass(err);
    
    MDB_EXEC(db, NULL, "INSERT INTO track %s", NULL, str.buf);
    string_clear(&str);

    return STATUS_OK;
}

static NEOERR* dyn_cmd_getadmin(struct queue_entry *q, struct cache *cd, mdb_conn *db)
{
    unsigned char *val = NULL; size_t vsize = 0;
    char *uname, *aname;
    int uid, aid;
    NEOERR *err;

    REQ_GET_PARAM_STR(q->hdfrcv, "uname", uname);
    REQ_GET_PARAM_STR(q->hdfrcv, "aname", aname);
    uid = hash_string(uname);
    aid = hash_string(aname);

    if (cache_getf(cd, &val, &vsize, PREFIX_ADMIN"%d_%d", uid, aid)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MDB_QUERY_RAW(db, "track", TRACK_COL, "uid=%d AND aid=%d AND type=%d "
                      " ORDER BY id DESC LIMIT 1;",
                      NULL, uid, aid, TYPE_JOIN);
        err = mdb_set_row(q->hdfsnd, db, TRACK_COL, NULL);
        nerr_handle(&err, NERR_NOT_FOUND);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, ONE_MINUTE, PREFIX_ADMIN"%d_%d", uid, aid);
    }
    
    return STATUS_OK;
}

static void dyn_process_driver(struct event_entry *entry, struct queue_entry *q)
{
    struct dyn_entry *e = (struct dyn_entry*)entry;
    NEOERR *err;
    int ret;
    
    mdb_conn *db = e->db;
    struct cache *cd = e->cd;
    struct dyn_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_ADDTRACK:
        err = dyn_cmd_addtrack(q, cd, db);
        break;
    case REQ_CMD_GETADMIN:
        err = dyn_cmd_getadmin(q, cd, db);
        break;
    case REQ_CMD_STATS:
        st->msg_stats++;
        err = STATUS_OK;
        hdf_set_int_value(q->hdfsnd, "msg_total", st->msg_total);
        hdf_set_int_value(q->hdfsnd, "msg_unrec", st->msg_unrec);
        hdf_set_int_value(q->hdfsnd, "msg_badparam", st->msg_badparam);
        hdf_set_int_value(q->hdfsnd, "msg_stats", st->msg_stats);
        hdf_set_int_value(q->hdfsnd, "proc_suc", st->proc_suc);
        hdf_set_int_value(q->hdfsnd, "proc_fai", st->proc_fai);
        break;
    default:
        st->msg_unrec++;
        err = nerr_raise(REP_ERR_UNKREQ, "unknown command %u", q->operation);
        break;
    }
    
    NEOERR *neede = mcs_err_valid(err);
    ret = neede ? neede->error : REP_OK;
    if (PROCESS_OK(ret)) {
        st->proc_suc++;
    } else {
        st->proc_fai++;
        if (ret == REP_ERR_BADPARAM) {
            st->msg_badparam++;
        }
        TRACE_ERR(q, ret, err);
    }
    if (q->req->flags & FLAGS_SYNC) {
        reply_trigger(q, ret);
    }
}

static void dyn_stop_driver(struct event_entry *entry)
{
    struct dyn_entry *e = (struct dyn_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mdb_destroy(e->db);
    cache_free(e->cd);
}



static struct event_entry* dyn_init_driver(void)
{
    struct dyn_entry *e = calloc(1, sizeof(struct dyn_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = dyn_process_driver;
    e->base.stop_driver = dyn_stop_driver;

    char *dbsn = hdf_get_value(g_cfg, CONFIG_PATH".dbsn", NULL);
    err = mdb_init(&e->db, dbsn);
    JUMP_NOK(err, error);
    
    e->cd = cache_create(hdf_get_int_value(g_cfg, CONFIG_PATH".numobjs", 1024), 0);
    if (e->cd == NULL) {
        wlog("init cache failure");
        goto error;
    }
    
    return (struct event_entry*)e;
    
error:
    if (e->base.name) free(e->base.name);
    if (e->db) mdb_destroy(e->db);
    if (e->cd) cache_free(e->cd);
    free(e);
    return NULL;
}

struct event_driver dyn_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = dyn_init_driver,
};
