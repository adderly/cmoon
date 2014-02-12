#ifndef __OSTAT_H__
#define __OSTAT_H__
#include "mheads.h"

__BEGIN_DECLS

NEOERR* stat_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OSTAT_H__ */
