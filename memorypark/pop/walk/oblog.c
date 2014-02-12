#include "mheads.h"
#include "lheads.h"
#include "oapp.h"
#include "oblog.h"

#define BLOG_COL " id, id%32 ||'/'|| id as fname, title, content, state, author, " \
    " to_char(intime, 'YYYY-MM-DD') as intime "

NEOERR* blog_index_static_get(HDF *hdf, HASH *dbh)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "aux");
    NEOERR *err;

    LPRE_DBOP(hdf, conn);

    /* set pgtt for caller use */
    MMISC_PAGEDIV_SET(hdf, PRE_OUTPUT, conn, "blog", " state=%d ",
                      NULL, BLOG_ST_NORMAL);
    int ntt = hdf_get_int_value(hdf, PRE_OUTPUT".ntt", 0);
    int pgtt = (ntt+BLOG_NUM_PERPAGE-1) / BLOG_NUM_PERPAGE;
    hdf_set_int_value(hdf, PRE_OUTPUT".pgtt", pgtt);
    
    int offset = 0;
    int pageid = hdf_get_int_value(hdf, PRE_QUERY".pageid", 0);
    if (pageid == 0)
        offset = (pgtt - 1) * BLOG_NUM_PERPAGE;
    else
        offset = (pageid-1) * BLOG_NUM_PERPAGE;
    MDB_QUERY_RAW(conn, "blog", BLOG_COL, " state=%d ORDER BY ID "
                  " LIMIT %d OFFSET %d", NULL,
                  BLOG_ST_NORMAL, BLOG_NUM_PERPAGE, offset);

    return nerr_pass(mdb_set_rows(hdf, conn, BLOG_COL, PRE_OUTPUT".blogs", "0"));
}

NEOERR* blog_static_get(HDF *hdf, HASH *dbh)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "aux");
    NEOERR *err;

    LPRE_DBOP(hdf, conn);

    int bid = hdf_get_int_value(hdf, PRE_QUERY".bid", 0);

    MDB_QUERY_RAW(conn, "blog", BLOG_COL, " id=%d AND state=%d ",
                  NULL, bid, BLOG_ST_NORMAL);
    err = mdb_set_row(hdf, conn, BLOG_COL, PRE_OUTPUT".blog");
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * previous
     * TODO id%32 ===> id%%d, BLOG_SUBDIR_NUM
     */
    MDB_QUERY_RAW(conn, "blog", " id%32 ||'/'|| id as id, title ",
                  " id < %d AND state=%d ORDER BY ID DESC LIMIT 1 ",
                  NULL, bid, BLOG_ST_NORMAL);
    err = mdb_set_row(hdf, conn, " fnameprev, titleprev ", PRE_OUTPUT".blog");
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * next 
     */
    MDB_QUERY_RAW(conn, "blog", " id%32 ||'/'|| id as id, title ",
                  " id > %d AND state=%d ORDER BY ID LIMIT 1 ",
                  NULL, bid, BLOG_ST_NORMAL);
    err = mdb_set_row(hdf, conn, " fnamenext, titlenext ", PRE_OUTPUT".blog");

    return nerr_pass(err);
}

NEOERR* blog_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "aux");
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *author;
    NEOERR *err;

    LPRE_DBOP(cgi->hdf, conn);

    APP_CHECK_LOGIN();

    if (hdf_get_int_value(evt->hdfrcv, "state", 0) < LCS_ST_ADMIN)
        return nerr_raise(LERR_LIMIT, "%s wan't be admin", aname);

    int bid = hdf_get_int_value(cgi->hdf, PRE_QUERY".bid", 0);
    /* just rend tpl */
    if (bid == 0) return STATUS_OK;

    MDB_QUERY_RAW(conn, "blog", BLOG_COL, " id=%d ",
                  NULL, bid, BLOG_ST_NORMAL);
    err = mdb_set_row(cgi->hdf, conn, BLOG_COL, PRE_OUTPUT".blog");
    if (err != STATUS_OK) return nerr_pass(err);

    author = hdf_get_value(cgi->hdf, PRE_OUTPUT".blog.author", NULL);
    if (!author) return nerr_raise(LERR_MISS_DATA, "%s not exist", aname);
    
    if (strcmp(aname, author))
        return nerr_raise(LERR_LIMIT, "%s not %s", aname, author);

    return STATUS_OK;
}

NEOERR* blog_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "aux");
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, command[1024];
    char *title, *content;
    int id = 0;
    NEOERR *err;

    LPRE_DBOP(cgi->hdf, conn);
    
    APP_CHECK_LOGIN();
    
    if (hdf_get_int_value(evt->hdfrcv, "state", 0) < LCS_ST_ADMIN)
        return nerr_raise(LERR_LIMIT, "%s wan't be admin", aname);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".title", title);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".content", content);

    MDB_EXEC(conn, NULL, "INSERT INTO blog (title, content, author) "
             " VALUES ($1::varchar(256), $2, "
             " $3::varchar(256)) RETURNING id",
             "sss", title, content, aname);
    err = mdb_get(conn, "i", &id);
    if (err != STATUS_OK) return nerr_pass(err);

    snprintf(command, sizeof(command), PATH_PAGER"blg -i 0 -b %d", id);
    mtc_dbg("%s", command);
    system(command);

    return STATUS_OK;
}

NEOERR* blog_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "aux");
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    int bid;
    NEOERR *err;

    LPRE_DBOP(cgi->hdf, conn);

    HDF_GET_INT(cgi->hdf, PRE_QUERY".bid", bid);

    APP_CHECK_LOGIN();
    
    if (hdf_get_int_value(evt->hdfrcv, "state", 0) < LCS_ST_ADMIN)
        return nerr_raise(LERR_LIMIT, "%s wan't be admin", aname);

    STRING str;
    string_init(&str);
    err = mcs_build_upcol(hdf_get_obj(cgi->hdf, PRE_QUERY),
                          hdf_get_obj(g_cfg, "Db.UpdateCol.blog"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(conn, NULL, "UPDATE blog SET %s WHERE id=%d AND author=$1",
             "s", str.buf, bid, aname);
    string_clear(&str);

    char command[1024];
    snprintf(command, sizeof(command), PATH_PAGER"blg -b %d", bid);
    mtc_dbg("%s", command);
    system(command);

    return STATUS_OK;
}

NEOERR* blog_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "aux");
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    int bid, state;
    NEOERR *err;

    LPRE_DBOP(cgi->hdf, conn);

    HDF_GET_INT(cgi->hdf, PRE_QUERY".bid", bid);
    HDF_GET_INT(cgi->hdf, PRE_QUERY".state", state);

    APP_CHECK_LOGIN();
    
    if (hdf_get_int_value(evt->hdfrcv, "state", 0) < LCS_ST_ADMIN)
        return nerr_raise(LERR_LIMIT, "%s wan't be admin", aname);

    MDB_EXEC(conn, NULL, "UPDATE blog SET state=%d WHERE id=%d AND "
             " author=$1", "s", state, bid, aname);

    char command[1024];
    snprintf(command, sizeof(command), PATH_PAGER"blg -b %d", bid);
    mtc_dbg("%s", command);
    system(command);

    if (state == BLOG_ST_DEL) {
        snprintf(command, sizeof(command), "rm -f %s/%d/%d.html",
                 PATH_BLOG, bid%BLOG_SUBDIR_NUM, bid);
        mtc_dbg("%s", command);
        system(command);
    }

    return STATUS_OK;
}

NEOERR* blog_mkdparser_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    hdf_set_copy(cgi->hdf, PRE_OUTPUT".s", PRE_QUERY".s");
    
    return STATUS_OK;
}
