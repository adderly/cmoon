/*
 * viki 主脑
 */

#include "mheads.h"
#include "lheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    CGI *cgi;
    NEOERR *err;
    int ret;

    HASH *dbh;
    HASH *tplh;
    session_t *session = NULL;
    char *requri, *jsoncb;

    int (*data_handler)(CGI *cgi, HASH *dbh, session_t *session);
    void *lib;

    //sleep(20);
    mconfig_parse_file(SITE_CONFIG, &g_cfg);
    mtc_init(TC_ROOT"viki");

    ret = ltpl_init(&tplh);
    if (ret != RET_RBTOP_OK) {
        mtc_err("init templates error");
        mutil_redirect("初始化模板失败", TGT_SELF, URL_CLOSE, true);
        return ret;
    }

    ret = ldb_init(&dbh);
    if (ret != RET_RBTOP_OK) {
        mtc_err("init db error");
        mutil_redirect("初始化数据库失败", TGT_SELF, URL_CLOSE, true);
        return ret;
    }

    lib = dlopen(NULL, RTLD_NOW|RTLD_GLOBAL);
    if (lib == NULL) {
        mtc_err("possible? %s", dlerror());
        mutil_redirect("初始化库函数失败", TGT_SELF, URL_CLOSE, true);
        return 1;
    }
    
#ifndef DROP_FCGI
    cgiwrap_init_emu(NULL, &read_cb, &printf_cb, &write_cb, NULL, NULL, NULL);
    while (FCGI_Accept() >= 0) {
#endif
        cgiwrap_init_std(argc, argv, environ);
        err = cgi_init(&cgi, NULL);
        JUMP_NOK_CGI(err, response);
        err = cgi_parse(cgi);
        JUMP_NOK_CGI(err, response);

#ifdef NCGI_MODE
        hdf_set_value(cgi->hdf, PRE_REQ_URI_RW, "/csc/hc");
        hdf_set_value(cgi->hdf, PRE_COOKIE".uin", "1001");
        hdf_set_value(cgi->hdf, PRE_COOKIE".uname", "bigml");
        hdf_set_value(cgi->hdf, PRE_COOKIE".musn", "8Y]u0|v=*MS]U3J");
#endif
        
        ret = session_init(cgi->hdf, dbh, &session);
        if (ret != RET_RBTOP_OK) {
            mtc_err("init session failure");
            goto response;
        }

        requri = hdf_get_value(cgi->hdf, PRE_REQ_URI_RW, "NULL");
        if (mutil_client_attack(cgi->hdf, requri, LMT_CLI_ATTACK,
                                PERIOD_CLI_ATTACK)) {
            goto response;
        }
        
        ret = lfile_access_rewrited(cgi, dbh, session);
        if (ret != RET_RBTOP_OK) {
            goto response;
        }

        data_handler = lutil_get_data_handler(lib, cgi);
        if (data_handler == NULL) {
            mtc_err("get handler failure");
            ret = RET_RBTOP_NEXIST;
            goto response;
        }

        ret = (*data_handler)(cgi, dbh, session);
        
    response:
        if (cgi != NULL && cgi->hdf != NULL) {
#ifdef DEBUG_HDF
            hdf_write_file(cgi->hdf, TC_ROOT"hdf.viki");
#endif
            switch (CGI_REQ_TYPE(cgi)) {
            case CGI_REQ_HTML:
                if (CGI_REQ_METHOD(cgi) != CGI_REQ_GET) {
                    goto resp_ajax;
                }
                if (ret != RET_RBTOP_OK && ret == RET_RBTOP_NEXIST) {
                    cgi_redirect(cgi, "/404.html");
                } else {
                    ret = ltpl_render(cgi, tplh, session);
                    if (ret != RET_RBTOP_OK) {
                        if (ret == RET_RBTOP_NEXIST)
                            cgi_redirect(cgi, "/404.html");
                        else
                            cgi_redirect(cgi, "/503.html");
                    }
                }
                break;
            case CGI_REQ_AJAX:
            resp_ajax:
                ldb_opfinish_json(ret, cgi->hdf, NULL, 0);
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
            cgi_destroy(&cgi);
            session_destroy(&session);
        }
#ifndef DROP_FCGI
    }
#endif

    ldb_destroy(dbh);
    ltpl_destroy(tplh);
    mconfig_cleanup(&g_cfg);
    return 0;
}
