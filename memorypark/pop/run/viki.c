/*
 * viki 主脑
 */

#include "mheads.h"
#include "lheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    CGI *cgi;
    NEOERR *err = STATUS_OK;

    HASH *dbh, *tplh, *evth;
    session_t *session = NULL;
    char *jsoncb;

    NEOERR* (*data_handler)(CGI *cgi, HASH *dbh, HASH *evth, session_t *session);
    void *lib;

    //sleep(20);
    mtc_init(TC_ROOT"viki");

    err = lerr_init();
    DIE_NOK_CGI(err);
    
    err = mconfig_parse_file(SITE_CONFIG, &g_cfg);
    DIE_NOK_CGI(err);

    err = ltpl_init(&tplh);
    DIE_NOK_CGI(err);

    err = ldb_init(&dbh);
    DIE_NOK_CGI(err);

    err = levt_init(&evth);
    DIE_NOK_CGI(err);

    lib = dlopen(NULL, RTLD_NOW|RTLD_GLOBAL);
    if (!lib) {
        err = nerr_raise(NERR_SYSTEM, "dlopen %s", dlerror());
        DIE_NOK_CGI(err);
    }
    
#ifndef DROP_FCGI
    cgiwrap_init_emu(NULL, &read_cb, &printf_cb, &write_cb, NULL, NULL, NULL);
    while (FCGI_Accept() >= 0) {
#endif
        cgiwrap_init_std(argc, argv, environ);
        err = cgi_init(&cgi, NULL);
        if (err != STATUS_OK) goto response;
        err = cgi_parse(cgi);
        if (err != STATUS_OK) goto response;

#ifdef NCGI_MODE
        hdf_set_value(cgi->hdf, PRE_REQ_URI_RW, "/blog/mkdparser");
        hdf_set_value(cgi->hdf, PRE_COOKIE".uin", "1001");
        hdf_set_value(cgi->hdf, PRE_COOKIE".uname", "bigml");
        hdf_set_value(cgi->hdf, PRE_COOKIE".musn", "8Y]u0|v=*MS]U3J");
        hdf_set_value(cgi->hdf, PRE_QUERY".ip", "222.247.56.14");
        hdf_set_value(cgi->hdf, PRE_QUERY".to", "cj_BXTSJ");
        hdf_set_value(cgi->hdf, PRE_QUERY".s", "koldddd");
        hdf_set_value(cgi->hdf, PRE_QUERY".JsonCallback", "Ape.transport.read");
#endif
        
        err = session_init(cgi, dbh, &session);
        if (err != STATUS_OK) goto response;

        if (lutil_client_attack(cgi->hdf, session, "lcs_uname")) {
            err = nerr_raise(LERR_ATTACK, "%s need a rest, babey!", session->dataer);
            goto response;
        }
        
        if ((data_handler = lutil_get_data_handler(lib, cgi, session)) == NULL) {
            err = nerr_raise(LERR_MISS_DATA, "dataer %s not found", session->dataer);
            goto response;
        }

        err = (*data_handler)(cgi, dbh, evth, session);
        
    response:
        if (cgi != NULL && cgi->hdf != NULL) {
            lerr_opfinish_json(err, cgi->hdf);
            
            switch (session->reqtype) {
            case CGI_REQ_HTML:
                err = ltpl_render(cgi, tplh, session);
                if (err != STATUS_OK) {
                    if (nerr_match(err, LERR_MISS_TPL))
                        cgi_redirect(cgi, "/404.html");
                    else
                        cgi_redirect(cgi, "/503.html");
                    TRACE_NOK(err);
                }
                break;
            case CGI_REQ_AJAX:
                jsoncb = hdf_get_value(cgi->hdf, PRE_REQ_AJAX_FN, NULL);
                if (jsoncb != NULL) {
                    mjson_execute_hdf(cgi->hdf, jsoncb, session->tm_cache_browser);
                } else {
                    mjson_output_hdf(cgi->hdf, session->tm_cache_browser);
                }
                break;
            default:
                cgi_redirect(cgi, "/503.html");
                break;
            }
            
#ifdef DEBUG_HDF
            hdf_write_file(cgi->hdf, TC_ROOT"hdf.viki");
#endif
            
            cgi_destroy(&cgi);
            session_destroy(&session);
        }
#ifndef DROP_FCGI
    }
#endif

    levt_destroy(evth);
    ldb_destroy(dbh);
    ltpl_destroy(tplh);
    mconfig_cleanup(&g_cfg);
    return 0;
}
