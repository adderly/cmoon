#ifndef __OMSG_H__
#define __OMSG_H__
#include "mheads.h"

__BEGIN_DECLS

NEOERR* msg_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OMSG_H__ */
