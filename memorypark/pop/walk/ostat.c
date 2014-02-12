#include "mheads.h"
#include "lheads.h"
#include "ostat.h"

NEOERR* stat_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "dyn");

    char *aname, *uname, *ip, *refer, *agent;

    aname = hdf_get_value(cgi->hdf, PRE_QUERY".aname", "unknown");
    uname = hdf_get_value(cgi->hdf, PRE_COOKIE".aname", NULL);
    if (!uname) uname = hdf_get_value(cgi->hdf, PRE_COOKIE".lcs_uname", "unknown");
    refer = hdf_get_value(cgi->hdf, "HTTP.Referer", " ");
    agent = hdf_get_value(cgi->hdf, "HTTP.UserAgent", "ie");
    ip = hdf_get_value(cgi->hdf, "CGI.RemoteAddress", "127.0.0.1");

    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_set_value(evt->hdfsnd, "oname", aname);
    hdf_set_value(evt->hdfsnd, "uname", uname);
    hdf_set_value(evt->hdfsnd, "refer", refer);
    hdf_set_value(evt->hdfsnd, "agent", agent);
    hdf_set_value(evt->hdfsnd, "ip", ip);
    hdf_set_int_value(evt->hdfsnd, "type", TYPE_VISIT);

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, aname, REQ_CMD_ADDTRACK, FLAGS_NONE);

    return STATUS_OK;
}
