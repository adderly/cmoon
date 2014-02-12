#ifndef __OADMIN_H__
#define __OADMIN_H__
#include "mheads.h"

__BEGIN_DECLS

/*
 * called after CHECK_XXX() immediately
 */
#define SET_ADMIN_ACTION()                                              \
    do {                                                                \
        hdf_set_value(cgi->hdf, PRE_WALK_SACTION".0", "actions_1");     \
        if (hdf_get_int_value(cgi->hdf, PRE_OUTPUT".member.verify", -1) >= MEMBER_VF_ADMIN) \
            hdf_set_value(cgi->hdf, PRE_WALK_SACTION".1", "actions_2"); \
    }                                                                   \
    while (0)

NEOERR* admin_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* admin_paper_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* admin_paper_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* admin_paper_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* admin_paper_new_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* admin_paper_edit_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);


NEOERR* admin_nav_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* admin_nav_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* admin_nav_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OADMIN_H__ */
