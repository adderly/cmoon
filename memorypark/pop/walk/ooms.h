#ifndef __OOMS_H__
#define __OOMS_H__
#include "mheads.h"

__BEGIN_DECLS

#define SET_ADMIN_ACTION(hdfin, hdfout)                                    \
    do {                                                                \
        if (hdf_get_int_value(hdfin, "pid", 1) == 0) {                    \
            hdf_set_value(hdfout, PRE_SPECIAL_ACTION".0", "actions_1"); \
        }                                                                \
        if (hdf_get_int_value(hdfin, "state", LCS_ST_FREE) == LCS_ST_ADMIN) { \
            hdf_set_value(hdfout, PRE_SPECIAL_ACTION".1", "actions_2"); \
        }                                                                \
        if (hdf_get_int_value(hdfin, "state", LCS_ST_FREE) == LCS_ST_ROOT) { \
            hdf_set_value(hdfout, PRE_SPECIAL_ACTION".1", "actions_3"); \
        }                                                                \
    } while (0)

NEOERR* oms_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* oms_camer_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* oms_edit_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* oms_edit_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* oms_users_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* oms_users_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* oms_users_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* oms_secy_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* oms_stat_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* oms_bill_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* oms_admin_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* oms_root_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OOMS_H__ */
