#include "mheads.h"
#include "lheads.h"
#include "oapp.h"

static void app_after_login(CGI *cgi, HASH *evth, char *aname)
{
    char tm[LEN_TM_GMT], *p, masn[LEN_CK];
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    if (!evt) return;

    memset(masn, 0x0, sizeof(masn));
    mcs_rand_string(masn, sizeof(masn));

    /*
     * set cookie 
     */
    cgi_cookie_set(cgi, "aname", aname, NULL, SITE_DOMAIN, NULL, 1, 0);
    neos_url_escape(aname, &p, NULL);
    cgi_cookie_set(cgi, "aname_esc", p, NULL, SITE_DOMAIN, NULL, 1, 0);
    free(p);

    //cgi_url_escape(masn, &p);
    mmisc_getdatetime_gmt(tm, sizeof(tm), "%A, %d-%b-%Y %T GMT", 60*60*3);
    cgi_cookie_set(cgi, "masn", masn, NULL, SITE_DOMAIN, tm, 1, 0);

    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_set_value(evt->hdfsnd, "masn", masn);
    MEVENT_TRIGGER_NRET(evt, aname, REQ_CMD_APPUP, FLAGS_NONE);

    hdf_set_copy(cgi->hdf, PRE_OUTPUT".aname", PRE_QUERY".aname");
}

NEOERR* app_exist_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    
    /*
     * input check
     */
    LPRE_EVTOP(cgi->hdf, evt);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".aname", aname);

    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "aname", aname);

    
    /*
     * trigger
     */
    mevent_trigger(evt, aname, REQ_CMD_APPINFO, FLAGS_SYNC);
    if (PROCESS_OK(evt->errcode)) {
        hdf_set_value(cgi->hdf, PRE_OUTPUT".exist", "1");
        hdf_set_value(cgi->hdf, PRE_OUTPUT".msg", "站点已被注册");
    } else if (evt->errcode == LERR_NREGIST) {
        hdf_set_value(cgi->hdf, PRE_OUTPUT".exist", "0");
        hdf_set_value(cgi->hdf, PRE_OUTPUT".msg", "站点还未注册");
    } else {
        return nerr_raise(evt->errcode, "get app %s info failure %d",
                          evt->ename, evt->errcode);
    }
    
    return STATUS_OK;
}

NEOERR* app_info_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_LOGIN();

    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* app_reset_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt;
    char *aname, *email, *sub, *content;
    char rlink[LEN_CK];
    
    HDF_GET_STR(cgi->hdf, PRE_QUERY".aname", aname);
    mcs_rand_string(rlink, sizeof(rlink));
    
    /*
     * app reset
     */
    evt = (mevent_t*)hash_lookup(evth, "aic");
    LPRE_EVTOP(cgi->hdf, evt);

    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_set_value(evt->hdfsnd, "rlink", rlink);

    MEVENT_TRIGGER(evt, aname, REQ_CMD_APP_SETRLINK, FLAGS_SYNC);

    email = hdf_get_value(evt->hdfrcv, "email", NULL);

    /*
     * input check
     */
    evt = (mevent_t*)hash_lookup(evth, "aux");
    LPRE_EVTOP(cgi->hdf, evt);

    sub = hdf_get_value(g_cfg, "Email.AppReset.subject", NULL);
    content = hdf_get_value(g_cfg, "Email.AppReset.content", NULL);

    if (!sub || !content) return nerr_raise(NERR_ASSERT, "param null");

    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "go", "1");
    hdf_set_value(evt->hdfsnd, "opt",
                  hdf_get_value(g_cfg, "Email.AppReset.opt", NULL));
    hdf_set_value(evt->hdfsnd, "sub", sub);
    hdf_set_value(evt->hdfsnd, "to", email);

    char *p = NULL;
    neos_url_escape(aname, &p, NULL);
    content = mmisc_str_repstr(2, content, "$aname$", aname, "$aname_esc$", p, "$rlink$", rlink);
    hdf_set_value(evt->hdfsnd, "content", content);
    SAFE_FREE(content);
    SAFE_FREE(p);
    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, email, REQ_CMD_MAIL_ADD, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* app_pass_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *rlink;
    NEOERR *err;

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".aname", aname);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".rlink", rlink);
    LPRE_EVTOP(cgi->hdf, evt);

    if (aname && rlink) {
        hdf_set_value(evt->hdfsnd, "aname", aname);
        MEVENT_TRIGGER(evt, aname, REQ_CMD_APP_GETRLINK, FLAGS_SYNC);

        char *s = hdf_get_value(evt->hdfrcv, "rlink", NULL);
        if (!s || strcmp(s, rlink))
            return nerr_raise(LERR_WRESET, "reset code error");

        app_after_login(cgi, evth, aname);
        
        hdf_set_value(cgi->hdf, PRE_OUTPUT".aname", aname);

        return STATUS_OK;
    }

    APP_CHECK_LOGIN();

    hdf_set_value(cgi->hdf, PRE_OUTPUT".aname", aname);
    
    return STATUS_OK;
}

NEOERR* app_new_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *asn, *email, masn[LEN_CK] = "init masn";
    
    /*
     * input check
     */
    LPRE_EVTOP(cgi->hdf, evt);
    
    HDF_GET_STR(cgi->hdf, PRE_QUERY".aname", aname);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".asn", asn);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".email", email);
    
    LEGAL_CK_ANAME(aname);
    LEGAL_CK_EMAIL(email);

    /*
     * prepare data 
     */
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    
    hdf_set_int_value(evt->hdfsnd, "state", LCS_ST_FREE);
    hdf_set_int_value(evt->hdfsnd, "tune", LCS_TUNE_EMAIL);
    hdf_set_value(evt->hdfsnd, "masn", masn);

    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, aname, REQ_CMD_APPNEW, FLAGS_SYNC);

    /*
     * follow-up
     */
    app_after_login(cgi, evth, aname);
    
    return STATUS_OK;
}

NEOERR* app_charge_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;

    APP_CHECK_LOGIN();

    hdf_copy(cgi->hdf, PRE_OUTPUT".appinfo", evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* app_charge_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    mevent_t *evtb = hash_lookup(evth, "bank");
    char *aname, *pname;
    int mpre;
    NEOERR *err;

    HDF_GET_INT(cgi->hdf, PRE_QUERY".mpre", mpre);

    APP_CHECK_LOGIN();

    pname = hdf_get_value(evt->hdfrcv, "pname", NULL);
    if (!pname) return nerr_raise(NERR_ASSERT, "pname null");

    hdf_set_value(evt->hdfsnd, "aname", pname);
    MEVENT_TRIGGER(evt, pname, REQ_CMD_APPINFO, FLAGS_SYNC);
    int state = hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_STRANGER);
    pname = hdf_get_value(evt->hdfrcv, "aname", NULL);

    /*
     * set ALL value, then trigger
     * because TRIGGER will del the hdfrcv, and pname error
     */
    hdf_set_value(evt->hdfsnd, "aname", pname);
    hdf_set_int_value(evt->hdfsnd, "state", LCS_ST_CHARGE);

    hdf_set_value(evtb->hdfsnd, "aname", pname);
    hdf_set_value(evtb->hdfsnd, "account", aname);
    hdf_set_int_value(evtb->hdfsnd, "fee", mpre);
    hdf_set_int_value(evtb->hdfsnd, "btype", BANK_OP_PRECHARGE);

    if (state < LCS_ST_CHARGE) {
        MEVENT_TRIGGER(evt, pname, REQ_CMD_APPUP, FLAGS_NONE);
    }

    MEVENT_TRIGGER(evtb, pname, REQ_CMD_BANK_ADDBILL, FLAGS_SYNC);

    return STATUS_OK;
}

NEOERR* app_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *asn;
    
    /*
     * input check
     */
    LPRE_EVTOP(cgi->hdf, evt);
    
    HDF_GET_STR(cgi->hdf, PRE_QUERY".aname", aname);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".asn", asn);
    
    LEGAL_CK_ANAME(aname);
    
    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "aname", aname);
    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, aname, REQ_CMD_APPINFO, FLAGS_SYNC);
    
    /*
     * verify
     */
    char *asndb = hdf_get_value(evt->hdfrcv, "asn", NULL);
    if (asndb) {
        if (!strcmp(asndb, asn)) {
            app_after_login(cgi, evth, aname);
            return STATUS_OK;
        } else 
            return nerr_raise(LERR_LOGINPSW, "password wrong %s %s", asndb, asn);
    }

    return nerr_raise(LERR_NREGIST, "app %s not exist", aname);
}

NEOERR* app_logout_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname;
    NEOERR *err;
    
    APP_CHECK_LOGIN();

    HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", aname);
    
    hdf_set_value(evt->hdfsnd, "aname", aname);
    hdf_set_value(evt->hdfsnd, "masn", "0");

    MEVENT_TRIGGER(evt, aname, REQ_CMD_APPUP, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* app_check_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "aic");
    char *aname, *masn;
    
    /*
     * input check
     */
    LPRE_EVTOP(cgi->hdf, evt);

    HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", aname);
    HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".masn", masn);
    LEGAL_CK_ANAME(aname);

    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "aname", aname);

    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, aname, REQ_CMD_APPINFO, FLAGS_SYNC);

    /*
     * verify
     */
    char *masndb = hdf_get_value(evt->hdfrcv, "masn", NULL);
    if (masndb) {
        if (!strcmp(masndb, masn)) {
            return STATUS_OK;
        }
    }
    mtc_warn("%s<====>%s", masndb, masn);

    return nerr_raise(LERR_NOTLOGIN, "%s not login", aname);
}
