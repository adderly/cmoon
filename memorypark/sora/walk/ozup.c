#include "mheads.h"
#include "lheads.h"
#include "ozup.h"

NEOERR* zup_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    char *mname;
    mevent_t *evt = hash_lookup(evth, "sodo");
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, evt);

    MEMBER_CHECK_LOGIN();

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_SODO_POINT_GET, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    
    mcs_set_int_attrr(cgi->hdf, PRE_OUTPUT".points",
                      "type", CNODE_TYPE_ARRAY);

    TRACE_HDF(cgi->hdf);
    
    return STATUS_OK;
}

NEOERR* zup_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    char *mname;
    mevent_t *evt = hash_lookup(evth, "sodo");
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, evt);

    MEMBER_CHECK_LOGIN();

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_SODO_POINT_ADD, FLAGS_NONE);

    hdf_remove_tree(cgi->hdf, PRE_OUTPUT".member");

    return STATUS_OK;
}
