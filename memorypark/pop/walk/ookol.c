#include "mheads.h"
#include "lheads.h"
#include "ookol.h"

#include "oapp.h"

#define KOL_COL " type, count, "                        \
    " ceil(date_part('epoch', intime)*1000) as intime "
//    "to_char(intime, 'YYYY-MM-DD') as intime "

NEOERR* okol_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "mtool");
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    STRING str;
    NEOERR *err;
    
    LPRE_DBOP(cgi->hdf, conn);
    
    APP_CHECK_LOGIN();

    if (hdf_get_int_value(evt->hdfrcv, "state", 0) != LCS_ST_ROOT)
        return nerr_raise(LERR_LIMIT, "%s wan't be root", aname);

    /*
     * build query conditon
     */
    string_init(&str);
    err = mcs_build_querycond(hdf_get_obj(cgi->hdf, PRE_QUERY),
                              hdf_get_obj(g_cfg, "Db.QueryCond.kol"),
                              &str, " intime > current_date - 7 AND type=0 ");
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * execute
     */
    MDB_QUERY_RAW(conn, "kol", KOL_COL, "%s ORDER BY intime", NULL, str.buf);
    string_clear(&str);
    err = mdb_set_rows(cgi->hdf, conn, KOL_COL, PRE_OUTPUT".sts", "0");
    if (err != STATUS_OK) return nerr_pass(err);

    hdf_set_attr(cgi->hdf, PRE_OUTPUT".sts", "type", "array");
    
    return STATUS_OK;
}
