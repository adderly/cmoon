#ifndef __LDB_H__
#define __LDB_H__
#include "mheads.h"

__BEGIN_DECLS

#define DB_DSN            (hdf_get_value(g_cfg, "Dbsn", "sqlite:../data/prddb"))

void ldb_opfinish_json(int ret, HDF *hdf, mdb_conn *conn);

__END_DECLS
#endif    /* __LDB_H__ */
