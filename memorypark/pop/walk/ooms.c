#include "mheads.h"
#include "lheads.h"
#include "ooms.h"

#include "oapp.h"

static void ips2places(HDF *hdf, HASH *evth)
{
    if (!hdf || !evth) return;

    mevent_t *evt = (mevent_t*)hash_lookup(evth, "place");
    mevent_t *evta = (mevent_t*)hash_lookup(evth, "aic");
    if (!evt || !evta) {
        mtc_err("can't find event");
        return;
    }

    char *p, *q;
    HDF *node, *rnode;
    STRING ip;

    string_init(&ip);
    node = hdf_obj_child(hdf);
    while (node) {
        p = hdf_get_value(node, "ip", NULL);
        q = hdf_get_value(node, "addr", NULL);
        if (p && (!q || !*q)) string_appendf(&ip, "%s,", p);
        
        node = hdf_obj_next(node);
    }

    if (ip.len <= 0) return;
    
    hdf_set_value(evt->hdfsnd, "ip", ip.buf);
    MEVENT_TRIGGER_VOID(evt, ip.buf, REQ_CMD_PLACEGET, FLAGS_SYNC);

    if (evt->hdfrcv) {
        node = hdf_obj_child(hdf);
        while (node) {
            rnode = hdf_obj_child(evt->hdfrcv);
            p = hdf_get_value(node, "ip", NULL);
            q = hdf_get_value(node, "addr", NULL);
            if (p && (!q || !*q)) {
                while (rnode) {
                    q = hdf_get_value(rnode, "ip", NULL);
                    if (q && !strcmp(p, q)) break;
                    else q = NULL;
                    
                    rnode = hdf_obj_next(rnode);
                }

                if (q) {
                    hdf_set_value(node, "addr", hdf_get_value(rnode, "c", NULL));
                    hdf_set_value(node, "area", hdf_get_value(rnode, "a", NULL));

                    hdf_set_value(evta->hdfsnd, "uid", hdf_get_value(node, "uid", 0));
                    hdf_set_value(evta->hdfsnd, "aid", hdf_get_value(node, "aid", 0));
                    hdf_set_value(evta->hdfsnd, "addr", hdf_get_value(rnode, "c", "Mars"));
                    MEVENT_TRIGGER_NRET(evta, NULL, REQ_CMD_APPUSERUP, FLAGS_NONE);
                } else {
                    mtc_warn("can't find addr for %s", p);
                }
            }
            
            node = hdf_obj_next(node);
        }
    }
    
    string_clear(&ip);
}

NEOERR* oms_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_LOGIN();
    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);
    
    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);

    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_set_int_value(evt->hdfsnd, "limit", MAX_USERS_PERADMIN);
    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, aname, REQ_CMD_APPUSERS, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    /*
     * TODO: remove ips2places() when userinfo's addr all filled
     */
    ips2places(hdf_get_obj(cgi->hdf, PRE_OUTPUT".userlist"), evth);
    
    return STATUS_OK;
}

NEOERR* oms_camer_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *uname;
    NEOERR *err;

    APP_CHECK_LOGIN();
    
    HDF_GET_STR(cgi->hdf, PRE_QUERY".uname", uname);
    LEGAL_CK_ANAME(uname);

    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_set_value(evt->hdfsnd, "uname", uname);

    MEVENT_TRIGGER(evt, aname, REQ_CMD_APPUSEROUT, FLAGS_NONE);

    evt = (mevent_t*)hash_lookup(evth, "msg");
    if (evt) {
        hdf_set_value(evt->hdfsnd, "name", uname);
        hdf_set_value(evt->hdfsnd, "name2", aname);
        MEVENT_TRIGGER(evt, uname, REQ_CMD_DEL_BOTH, FLAGS_NONE);
    }

    return STATUS_OK;
}

NEOERR* oms_edit_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;

    APP_CHECK_ADMIN();

    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);

    err = oms_data_get(cgi, dbh, evth, ses);
    int tune = hdf_get_int_value(cgi->hdf, PRE_OUTPUT".appinfo.tune", 0);

    if (tune & LCS_TUNE_QUIET)
        hdf_set_value(cgi->hdf, PRE_OUTPUT".appinfo.quiet", "1");
    if (tune & LCS_TUNE_SMS)
        hdf_set_value(cgi->hdf, PRE_OUTPUT".appinfo.sms", "1");

    return nerr_pass(err);
}

NEOERR* oms_edit_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_LOGIN();
    
    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, aname, REQ_CMD_APPUP, FLAGS_NONE);
    
    return STATUS_OK;
}

NEOERR* oms_users_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_ADMIN();

    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);
    
    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);
    
    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "pname", aname);
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, aname, REQ_CMD_APP_O_USERS, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    
    return STATUS_OK;
}

NEOERR* oms_users_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *pname, *email;
    int cost;
    NEOERR *err;

    APP_CHECK_ADMIN();
    
    HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", pname);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".aname", aname);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".email", email);
    LEGAL_CK_ANAME(pname);
    LEGAL_CK_ANAME(aname);
    LEGAL_CK_EMAIL(email);

    /*
     * check
     */
    int state = hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_FREE);
    if (state <= LCS_ST_FREE)
        return nerr_raise(LERR_NEEDUP, "%s want to add users %s", pname, aname);
    else if (state >= LCS_ST_VIP) goto add;

    /*
     * chargeback
     */
    cost = hdf_get_int_value(g_cfg, "Cost.account", 0);
    evt = (mevent_t*)hash_lookup(evth, "bank");
    hdf_set_value(evt->hdfsnd, "aname", pname);
    hdf_set_int_value(evt->hdfsnd, "btype", BANK_OP_ADDACCOUNT);
    hdf_set_int_value(evt->hdfsnd, "fee", cost);
    hdf_set_value(evt->hdfsnd, "account", aname);

    MEVENT_TRIGGER(evt, pname, REQ_CMD_BANK_ADDBILL, FLAGS_SYNC);

add:
    /*
     * add
     */
    evt = (mevent_t*)hash_lookup(evth, "aic");
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    hdf_set_value(evt->hdfsnd, "pname", pname);
    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_set_int_value(evt->hdfsnd, "state",
                      hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_FREE));
    hdf_set_value(evt->hdfsnd, "masn", aname);

    if (PROCESS_NOK(mevent_trigger(evt, aname, REQ_CMD_APPNEW, FLAGS_SYNC))) {
        char *zpa = NULL;
        hdf_write_string(evt->hdfrcv, &zpa);
        mtc_foo("add %s failure %d %s", aname, evt->errcode, zpa);
        SAFE_FREE(zpa);

        if (state < LCS_ST_VIP) {
            /*
             * roll back
             */
            evt = (mevent_t*)hash_lookup(evth, "bank");
            hdf_set_value(evt->hdfsnd, "aname", pname);
            hdf_set_int_value(evt->hdfsnd, "btype", BANK_OP_ROLLBACK);
            hdf_set_int_value(evt->hdfsnd, "fee", -cost);
            hdf_set_valuef(evt->hdfsnd, "remark = 客服帐号 %s 创建失败", aname);

            if (PROCESS_NOK(mevent_trigger(evt, pname, REQ_CMD_BANK_ADDBILL,
                                           FLAGS_SYNC))) {
                /*
                 * ATTENTION we need pay back to customer manually
                 */
                hdf_write_string(evt->hdfrcv, &zpa);
                mtc_foo("rollback %s failure %d %s", aname, evt->errcode, zpa);
                SAFE_FREE(zpa);
            }
        }

        return nerr_raise(evt->errcode, "add %s failure %d", aname, evt->errcode);
    }
    
    return STATUS_OK;
}

NEOERR* oms_users_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *uname, *aname;
    NEOERR *err;

    HDF_GET_STR(cgi->hdf, PRE_QUERY".aname", uname);
    APP_CHECK_ADMIN_OTHER(uname);

    if (!strcmp(aname, uname))
        return nerr_raise(LERR_LIMIT, "%s want kill him self", aname);
    
    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "aname", uname);
    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, uname, REQ_CMD_APPDEL, FLAGS_SYNC);
    
    return STATUS_OK;
}

NEOERR* oms_secy_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *uname;
    NEOERR *err;

    HDF_GET_STR(cgi->hdf, PRE_QUERY".aname", uname);
    
    APP_CHECK_ADMIN_OTHER(uname);

    hdf_set_value(evt->hdfsnd, "pname", aname);
    hdf_set_value(evt->hdfsnd, "aname", uname);

    MEVENT_TRIGGER(evt, aname, REQ_CMD_APP_SETSECY, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* oms_stat_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    mevent_t *evtm = (mevent_t*)hash_lookup(evth, "mtls");
    char *aname;
    NEOERR *err;

    APP_CHECK_LOGIN();

    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);

    hdf_set_value(evtm->hdfsnd, "aname", aname);
    hdf_copy(evtm->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evtm, aname, REQ_CMD_GETSTAT, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evtm->hdfrcv);
    
    return STATUS_OK;
}

NEOERR* oms_bill_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;

    APP_CHECK_ADMIN();
    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);

    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);
    
    int state = hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_FREE);
    if (state <= LCS_ST_FREE) return STATUS_OK;

    evt = hash_lookup(evth, "bank");
    hdf_set_value(evt->hdfsnd, "aname", aname);
    MEVENT_TRIGGER(evt, aname, REQ_CMD_BANK_INFO, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT".bankinfo", evt->hdfrcv);

    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    MEVENT_TRIGGER(evt, aname, REQ_CMD_BANK_GETBILL, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    
    return STATUS_OK;
}

NEOERR* oms_admin_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;

    APP_CHECK_STUFF();
    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);

    return STATUS_OK;
}

NEOERR* oms_root_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;

    APP_CHECK_ROOT();
    SET_ADMIN_ACTION(evt->hdfrcv, cgi->hdf);

    return STATUS_OK;
}
