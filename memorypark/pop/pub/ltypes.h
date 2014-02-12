#ifndef __LTYPES_H__
#define __LTYPES_H__

#include "mheads.h"

__BEGIN_DECLS

typedef struct _session {
    int reqtype;
    time_t tm_cache_browser;
    char *uname;
    char *dataer;
    char *render;
} session_t;

NEOERR* session_init(CGI *cgi, HASH *dbh, session_t **ses);
void session_destroy(session_t **ses);

__END_DECLS
#endif    /* __LTYPES_H__ */
