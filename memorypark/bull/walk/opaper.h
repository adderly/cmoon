#ifndef __OPAPER_H__
#define __OPAPER_H__
#include "mheads.h"

__BEGIN_DECLS

#define _COL_PAPER "id, pid, statu, title, keyword, des, content, "     \
    " to_char(uptime, 'YYYY-MM-DD') as inday, "                         \
    " to_char(uptime, 'YYYY-MM-DD HH:mm:SS') as intime"                 \
    
enum {
    PAPER_ST_OK = 0,
    PAPER_ST_PAUSE,
    PAPER_ST_DEL
};

NEOERR* paper_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* index_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* paper_class_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* paper_matchtitle_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

NEOERR* paper_preview_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OPAPER_H__ */
