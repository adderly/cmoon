#include "mheads.h"
#include "lheads.h"
#include "ocomment.h"

#include "oapp.h"

NEOERR* comment_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aux");

    LPRE_EVTOP(cgi->hdf, evt);
    
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    MEVENT_TRIGGER(evt, NULL, REQ_CMD_CMT_GET, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    
    return STATUS_OK;
}

NEOERR* comment_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aux");
    mevent_t *evt_place = (mevent_t*)hash_lookup(evth, "place");

    LPRE_EVTOP(cgi->hdf, evt);

    /*
     * TODO other user can't comment on my improve...
     */
#if 0
    HDF_GET_INT(cgi->hdf, PRE_QUERY".type", type);
    if (type == CMT_TYPE_IMPROVE) {
        APP_CHECK_LOGIN();
        state = hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_FREE);
        if (state < LCS_ST_ADMIN && ())
    }
#endif
    
    char *ip = hdf_get_value(cgi->hdf, "CGI.RemoteAddress", "unknownHost");
    hdf_set_value(evt_place->hdfsnd, "ip", ip);
    MEVENT_TRIGGER_NRET(evt_place, ip, REQ_CMD_PLACEGET, FLAGS_SYNC);
    char *addr = hdf_get_value(evt_place->hdfrcv, "0.c", "火星");
    
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    hdf_set_value(evt->hdfsnd, "ip", ip);
    hdf_set_value(evt->hdfsnd, "addr", addr);
    MEVENT_TRIGGER(evt, NULL, REQ_CMD_CMT_ADD, FLAGS_SYNC);
    
    return STATUS_OK;
}

NEOERR* comment_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    return STATUS_OK;
}

NEOERR* comment_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;

    LPRE_EVTOP(cgi->hdf, evt);
    
    APP_CHECK_LOGIN();
    
    if (hdf_get_int_value(evt->hdfrcv, "state", 0) < LCS_ST_ADMIN)
        return nerr_raise(LERR_LIMIT, "%s wan't be admin", aname);

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    MEVENT_TRIGGER(evt, NULL, REQ_CMD_CMT_DEL, FLAGS_NONE);
    
    return STATUS_OK;
}
