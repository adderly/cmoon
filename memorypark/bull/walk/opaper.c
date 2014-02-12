#include "mheads.h"
#include "lheads.h"
#include "opaper.h"

NEOERR* paper_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname;
    int id;
    NEOERR *err;

    HDF_GET_INT(cgi->hdf, PRE_QUERY".id", id);

    /*
     * paper
     */
    MDB_QUERY_RAW(db, "paper", _COL_PAPER, "id=%d AND statu=%d",
                  NULL, id, PAPER_ST_OK);
    err = mdb_set_row(cgi->hdf, db, _COL_PAPER, PRE_OUTPUT".paper",
                      MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * papers
     */
    int count, offset;

    mdb_pagediv(cgi->hdf, PRE_QUERY, &count, &offset, PRE_OUTPUT, cgi->hdf);
    MDB_PAGEDIV_SET(cgi->hdf, PRE_OUTPUT, db, "paper", "pid=%d AND statu=%d",
                    NULL, id, PAPER_ST_OK);

    MDB_QUERY_RAW(db, "paper", _COL_PAPER, "pid=%d AND statu=%d",
                  NULL, id, PAPER_ST_OK);
    err = mdb_set_rows(cgi->hdf, db, _COL_PAPER, PRE_OUTPUT".papers",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * PRE_LAYOUT
     */
    hdf_set_copy(cgi->hdf, PRE_LAYOUT".title", PRE_OUTPUT".paper.title");
    
    return nerr_pass(nav_data_get(cgi, dbh, evth, ses));
}

NEOERR* index_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    NEOERR *err;

    MDB_QUERY_RAW(db, "paper", _COL_PAPER, "pid=4 AND statu=%d",
                  NULL, PAPER_ST_OK);
    err = mdb_set_rows(cgi->hdf, db, _COL_PAPER, PRE_OUTPUT".papersone",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_QUERY_RAW(db, "paper", _COL_PAPER, "pid=3 AND statu=%d",
                  NULL, PAPER_ST_OK);
    err = mdb_set_rows(cgi->hdf, db, _COL_PAPER, PRE_OUTPUT".paperstwo",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_QUERY_RAW(db, "paper", _COL_PAPER, "pid=6 AND statu=%d",
                  NULL, PAPER_ST_OK);
    err = mdb_set_rows(cgi->hdf, db, _COL_PAPER, PRE_OUTPUT".papersthree",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    hdf_set_value(cgi->hdf, PRE_QUERY".id", "1");
    hdf_set_value(cgi->hdf, PRE_QUERY"._tab", "0");

    return nerr_pass(paper_data_get(cgi, dbh, evth, ses));
}

NEOERR* paper_class_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char tok[LEN_HDF_KEY];
    int id, pid;
    NEOERR *err;

    HDF_FETCH_INT(cgi->hdf, PRE_QUERY".id", id);
    HDF_FETCH_INT(cgi->hdf, PRE_QUERY".pid", pid);

    HDF *node = hdf_get_child(cgi->hdf, PRE_QUERY".papers");
    if (node) {
    redo:
        HDF_GET_INT(node, "id", id);
        HDF_GET_INT(node, "pid", pid);
    }

    if (pid == 0) {
        hdf_set_valuef(cgi->hdf, PRE_OUTPUT".papers.%d.title=顶级分类", id);
    } else {
        MDB_QUERY_RAW(db, "paper", "title", "id=%d", NULL, pid);
        
        snprintf(tok, sizeof(tok), "%s.papers.%d", PRE_OUTPUT, id);
        err = mdb_set_row(cgi->hdf, db, "title", tok, MDB_FLAG_EMPTY_OK);
        if (err != STATUS_OK) return nerr_pass(err);
    }
    
    hdf_set_valuef(cgi->hdf, PRE_OUTPUT".papers.%d.id=%d", id, id);
    hdf_set_valuef(cgi->hdf, PRE_OUTPUT".papers.%d.pid=%d", id, pid);

    node = hdf_obj_next(node);
    if (node) goto redo;
    
    hdf_set_attr(cgi->hdf, PRE_OUTPUT".papers", "type", "array");
    
    return STATUS_OK;
}

NEOERR* paper_matchtitle_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *title;
    NEOERR *err;

    HDF_GET_STR(cgi->hdf, PRE_QUERY".title", title);

    MDB_QUERY_RAW(db, "paper", "id, title", "statu=%d AND "
                  " title LIKE '%%%s%%'", NULL, PAPER_ST_OK, title);
    err = mdb_set_rows(cgi->hdf, db, "id, title", PRE_OUTPUT".titles",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    hdf_set_attr(cgi->hdf, PRE_OUTPUT".titles", "type", "array");

    return STATUS_OK;
}

NEOERR* paper_preview_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    MCS_NOT_NULLA(cgi->hdf);

    hdf_set_copy(cgi->hdf, PRE_OUTPUT".s", PRE_QUERY".s");

    mstr_script_escape(cgi->hdf, PRE_OUTPUT".s");
    
    return STATUS_OK;
}
