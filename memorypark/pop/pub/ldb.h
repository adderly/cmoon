#ifndef __LDB_H__
#define __LDB_H__
#include "mheads.h"

__BEGIN_DECLS


#define LPRE_ALLOP(hdf, conn, evt)                                        \
    if (!hdf || !conn || !evt) return nerr_raise(NERR_ASSERT, "paramter null");

#define LPRE_DBOP(hdf, conn)                                            \
    if (!hdf || !conn) return nerr_raise(NERR_ASSERT, "paramter null");

#define LPRE_EVTOP(hdf, evt)                                            \
    if (!hdf || !evt) return nerr_raise(NERR_ASSERT, "paramter null");

NEOERR* ldb_init(HASH **dbh);
void ldb_destroy(HASH *dbh);

__END_DECLS
#endif    /* __LDB_H__ */
