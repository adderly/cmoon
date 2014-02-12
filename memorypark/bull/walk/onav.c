#include "mheads.h"
#include "lheads.h"
#include "onav.h"

NEOERR* nav_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    int tab;
    NEOERR *err;

    HDF_FETCH_INT(cgi->hdf, PRE_QUERY"._tab", tab);
    hdf_set_int_value(cgi->hdf, PRE_LAYOUT".tabpart", tab);

    MDB_QUERY_RAW(db, "nav n, paper p", _COL_NAV, "n.statu=%d AND "
                  " n.pid=p.id ORDER BY n.pos", NULL, NAV_ST_OK);
    err = mdb_set_rows(cgi->hdf, db, _COL_NAV, PRE_LAYOUT".tabs",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    int cnt = 0;
    
    HDF *node = hdf_get_child(cgi->hdf, PRE_LAYOUT".tabs");
    while (node) {
        hdf_set_copy(node, "name", "title");
        if (cnt == 0) {
            hdf_set_valuef(node, "href=/index?_tab=%d&id=%s",
                           cnt++, hdf_get_value(node, "pid", NULL));
        } else {
            hdf_set_valuef(node, "href=/paper?_tab=%d&id=%s",
                           cnt++, hdf_get_value(node, "pid", NULL));
        }
        
        node = hdf_obj_next(node);
    }

    return STATUS_OK;
}
