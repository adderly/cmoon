#ifndef __OCOMMENT_H__
#define __OCOMMENT_H__
#include "mheads.h"

__BEGIN_DECLS

NEOERR* comment_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* comment_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* comment_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* comment_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OCOMMENT_H__ */
