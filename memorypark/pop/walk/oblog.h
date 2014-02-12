#ifndef __OBLOG_H__
#define __OBLOG_H__
#include "mheads.h"

__BEGIN_DECLS

#define BLOG_SUBDIR_NUM        32
#define BLOG_NUM_PERPAGE    10

typedef enum {
    BLOG_ST_NORMAL = 0,
    BLOG_ST_DEL
} blog_state;

NEOERR* blog_index_static_get(HDF *hdf, HASH *dbh);
NEOERR* blog_static_get(HDF *hdf, HASH *dbh);

NEOERR* blog_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* blog_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* blog_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* blog_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/* post here */
NEOERR* blog_mkdparser_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OBLOG_H__ */
