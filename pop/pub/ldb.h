#ifndef __LDB_H__
#define __LDB_H__
#include "mheads.h"

__BEGIN_DECLS

#define LPRE_DBOP(hdf, conn, evt)						\
	if (hdf == NULL) {									\
		mtc_err("hdf is null");							\
		return RET_RBTOP_HDFNINIT;						\
	}													\
	if (mdb_get_errcode(conn) != MDB_ERR_NONE) {		\
		mtc_err("conn err %s", mdb_get_errmsg(conn));	\
		return RET_RBTOP_DBNINIT;						\
	}													\
	if (evt == NULL) {									\
		mtc_err("evt is null");							\
		return RET_RBTOP_EVTNINIT;						\
	}

#define LPRE_DBOP_NRET(hdf, conn, evt)					\
	if (hdf == NULL) {									\
		mtc_err("hdf is null");							\
		return;											\
	}													\
	if (mdb_get_errcode(conn) != MDB_ERR_NONE) {		\
		mtc_err("conn err %s", mdb_get_errmsg(conn));	\
		return;											\
	}													\
	if (evt == NULL) {									\
		mtc_err("evt is null");							\
		return;											\
	}

#define TABLE_RLS_USER	(hdf_get_value(g_cfg, "Db.Table.release_user", "rls_user_4"))

void ldb_opfinish(int ret, HDF *hdf, mdb_conn *conn,
				  char *target, char *url, bool header);
void ldb_opfinish_json(int ret, HDF *hdf, mdb_conn *conn, time_t second);

int  ldb_init(HASH **dbh);
void ldb_destroy(HASH *dbh);

__END_DECLS
#endif	/* __LDB_H__ */
