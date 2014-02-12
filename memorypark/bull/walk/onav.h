#ifndef __ONAV_H__
#define __ONAV_H__
#include "mheads.h"

__BEGIN_DECLS

#define _COL_NAV "n.pos AS pos, n.pid AS pid, p.title AS title"

enum {
    NAV_ST_OK = 0,
    NAV_ST_PAUSE,
    NAV_ST_DEL
};

NEOERR* nav_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __ONAV_H__ */
