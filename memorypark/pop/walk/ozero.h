#ifndef __OZERO_H__
#define __OZERO_H__
#include "mheads.h"

__BEGIN_DECLS

NEOERR* zero_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* zero_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* zero_detail_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OZERO_H__ */
