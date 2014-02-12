#include "mheads.h"
#include "lheads.h"
#include "oadmin.h"

NEOERR* admin_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    SET_ADMIN_ACTION();

    return nerr_pass(nav_data_get(cgi, dbh, evth, ses));
}

/*
 * paper
 */
NEOERR* admin_paper_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    int count, offset;
    char *mname;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    SET_ADMIN_ACTION();

    mdb_pagediv(cgi->hdf, PRE_QUERY, &count, &offset, PRE_OUTPUT, cgi->hdf);

    MDB_PAGEDIV_SET(cgi->hdf, PRE_OUTPUT, db, "paper", "statu=%d",
                    NULL, PAPER_ST_OK);
    
    MDB_QUERY_RAW(db, "paper", _COL_PAPER, "statu=%d ORDER BY id DESC "
                  " LIMIT %d OFFSET %d",
                  NULL, PAPER_ST_OK, count, offset);
    err = mdb_set_rows(cgi->hdf, db, _COL_PAPER, PRE_OUTPUT".papers",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);
    
    return nerr_pass(nav_data_get(cgi, dbh, evth, ses));
}

NEOERR* admin_paper_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    STRING str; string_init(&str);
    char *mname;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    err = mdb_build_incol(hdf_get_obj(cgi->hdf, PRE_QUERY),
                          hdf_get_obj(g_cfg, "Db.InsertCol.admin.paper"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "INSERT INTO paper %s", NULL, str.buf);

    string_clear(&str);

    return STATUS_OK;
}

NEOERR* admin_paper_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    STRING str; string_init(&str);
    char *mname;
    int id;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    HDF_GET_INT(cgi->hdf, PRE_QUERY".id", id);

    err = mdb_build_upcol(hdf_get_obj(cgi->hdf, PRE_QUERY),
                          hdf_get_obj(g_cfg, "Db.UpdateCol.admin.paper"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "UPDATE paper SET %s WHERE id=%d", NULL, str.buf, id);

    string_clear(&str);

    return STATUS_OK;
}

NEOERR* admin_paper_new_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    SET_ADMIN_ACTION();

    return nerr_pass(nav_data_get(cgi, dbh, evth, ses));
}

NEOERR* admin_paper_edit_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname;
    int id;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    SET_ADMIN_ACTION();

    HDF_GET_INT(cgi->hdf, PRE_QUERY".id", id);

    MDB_QUERY_RAW(db, "paper", _COL_PAPER, "id=%d", NULL, id);

    err = mdb_set_row(cgi->hdf, db, _COL_PAPER, PRE_OUTPUT".paper", MDB_FLAG_Z);
    if (err != STATUS_OK) return nerr_pass(err);

    return nerr_pass(nav_data_get(cgi, dbh, evth, ses));
}

/*
 * nav
 */
NEOERR* admin_nav_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    SET_ADMIN_ACTION();

    MDB_QUERY_RAW(db, "nav n, paper p", _COL_NAV, "n.statu=%d AND "
                  " n.pid=p.id ORDER BY n.pos", NULL, NAV_ST_OK);
    err = mdb_set_rows(cgi->hdf, db, _COL_NAV, PRE_OUTPUT".navs",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    return nerr_pass(nav_data_get(cgi, dbh, evth, ses));
}

NEOERR* admin_nav_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    STRING str; string_init(&str);
    char *mname;
    int pos;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    HDF_GET_INT(cgi->hdf, PRE_QUERY".pos", pos);

    err = mdb_build_upcol(hdf_get_obj(cgi->hdf, PRE_QUERY),
                          hdf_get_obj(g_cfg, "Db.UpdateCol.admin.nav"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "UPDATE nav SET %s WHERE pos=%d", NULL, str.buf, pos);

    string_clear(&str);

    return STATUS_OK;
}

NEOERR* admin_nav_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    STRING str; string_init(&str);
    char *mname;
    NEOERR *err;

    MEMBER_CHECK_ADMIN();

    err = mdb_build_incol(hdf_get_obj(cgi->hdf, PRE_QUERY),
                          hdf_get_obj(g_cfg, "Db.InsertCol.admin.nav"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "INSERT INTO nav %s", NULL, str.buf);

    string_clear(&str);

    return STATUS_OK;
}
