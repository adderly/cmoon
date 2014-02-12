#ifndef __LUTIL_H__
#define __LUTIL_H__
#include "mheads.h"

__BEGIN_DECLS

#define UIN_ILLEGAL(u)    (u < MIN_USER_NUM)
#define GID_ILLEGAL(g)    (g < 0)

typedef enum {
    CGI_REQ_HTML = 0,
    CGI_REQ_AJAX,
    CGI_REQ_UNSUPPORT
} cgi_req_type;

void* lutil_get_data_handler(void *lib, CGI *cgi, session_t *ses);
NEOERR* lutil_fetch_count(HDF *hdf, mdb_conn *conn, char *table, char *cond);
NEOERR* lutil_fetch_countf(HDF *hdf, mdb_conn *conn, char *table, char *cfmt, ...);
bool lutil_client_attack(HDF *hdf, session_t *ses, char *cname);

__END_DECLS
#endif    /* __LUTIL_H__ */
