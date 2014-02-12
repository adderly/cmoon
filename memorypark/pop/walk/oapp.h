#ifndef __OAPP_H__
#define __OAPP_H__
#include "mheads.h"

__BEGIN_DECLS

#define APP_CHECK_LOGIN()                                        \
    do {                                                        \
        err = app_check_login_data_get(cgi, dbh, evth, ses);    \
        if (err != STATUS_OK) return nerr_pass(err);            \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", aname);    \
        LEGAL_CK_ANAME(aname);                                    \
    } while (0)

#define APP_CHECK_ADMIN()                                            \
    do {                                                            \
        err = app_check_login_data_get(cgi, dbh, evth, ses);        \
        if (err != STATUS_OK) return nerr_pass(err);                \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", aname);        \
        LEGAL_CK_ANAME(aname);                                        \
        if (hdf_get_int_value(evt->hdfrcv, "pid", -1) != 0) {        \
            return nerr_raise(LERR_LIMIT, "%s not admin", aname);    \
        }                                                            \
    } while (0)

#define APP_CHECK_ADMIN_OTHER(uname)                                    \
    do {                                                                \
        err = app_check_login_data_get(cgi, dbh, evth, ses);            \
        if (err != STATUS_OK) return nerr_pass(err);                    \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", aname);            \
        LEGAL_CK_ANAME(aname);                                            \
        if (hdf_get_int_value(evt->hdfrcv, "pid", -1) != 0) {            \
            return nerr_raise(LERR_LIMIT, "%s not admin", aname);        \
        }                                                                \
        /* we can also get uname's info instead of get aname's ousers */ \
        hdf_set_value(evt->hdfsnd, "pname", aname);                        \
        MEVENT_TRIGGER(evt, aname, REQ_CMD_APP_O_USERS, FLAGS_SYNC); \
        if (!hdf_get_valuef(evt->hdfrcv, "users.%s.aname", uname)) {    \
            return nerr_raise(LERR_LIMIT, "%s not %s admin", aname, uname);    \
        }                                                                \
    } while (0)

#define APP_CHECK_STUFF()                                            \
    do {                                                            \
        err = app_check_login_data_get(cgi, dbh, evth, ses);        \
        if (err != STATUS_OK) return nerr_pass(err);                \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", aname);        \
        LEGAL_CK_ANAME(aname);                                        \
        if (hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_FREE) < LCS_ST_ADMIN) {    \
            return nerr_raise(LERR_LIMIT, "%s not admin", aname);    \
        }                                                            \
    } while (0)

#define APP_CHECK_ROOT()                                            \
    do {                                                            \
        err = app_check_login_data_get(cgi, dbh, evth, ses);        \
        if (err != STATUS_OK) return nerr_pass(err);                \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".aname", aname);        \
        LEGAL_CK_ANAME(aname);                                        \
        if (hdf_get_int_value(evt->hdfrcv, "state", LCS_ST_FREE) != LCS_ST_ROOT) { \
            return nerr_raise(LERR_LIMIT, "%s not admin", aname);        \
        }                                                                \
    } while (0)

NEOERR* app_exist_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* app_info_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* app_reset_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* app_pass_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* app_new_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* app_charge_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* app_charge_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
/* sb want to login */
NEOERR* app_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* app_logout_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
/* sb want to do sth serious, need be verify */
NEOERR* app_check_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);


__END_DECLS
#endif /* __OAPP_H__ */
