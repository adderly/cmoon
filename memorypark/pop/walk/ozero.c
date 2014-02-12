#include "mheads.h"
#include "lheads.h"
#include "ozero.h"
#include "oapp.h"

NEOERR* zero_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_LOGIN();
    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);
    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);

    evt = hash_lookup(evth, "aux");
    hdf_set_value(evt->hdfsnd, "aname", aname);
    MEVENT_TRIGGER(evt, aname, REQ_CMD_IMP_GET, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* zero_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_LOGIN();
    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);
    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);

    evt = hash_lookup(evth, "aux");
    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    MEVENT_TRIGGER(evt, aname, REQ_CMD_IMP_ADD, FLAGS_SYNC);

    return STATUS_OK;
}

NEOERR* zero_detail_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_LOGIN();
    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);
    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);
    int state = hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_FREE);

    evt = hash_lookup(evth, "aux");
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    MEVENT_TRIGGER(evt, aname, REQ_CMD_IMP_DETAIL, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);

    char *anamedb = hdf_get_value(evt->hdfrcv, "aname", NULL);
    if (state < LCS_ST_ADMIN && (!anamedb || strcmp(anamedb, aname)))
        return nerr_raise(LERR_LIMIT, "not %s ticket", aname);

    return STATUS_OK;
}
