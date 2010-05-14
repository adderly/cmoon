#include "mevent_plugin.h"
#include "mevent_aic.h"

#define PLUGIN_NAME	"aic"
#define CONFIG_PATH	PRE_PLUGIN"."PLUGIN_NAME

struct aic_stats {
	unsigned long msg_total;
	unsigned long msg_unrec;
	unsigned long msg_badparam;
	unsigned long msg_stats;

	unsigned long proc_suc;
	unsigned long proc_fai;
};

struct aic_entry {
	struct event_entry base;
	mdb_conn *db;
	struct cache *cd;
	struct aic_stats st;
};

/*
 * input : aid(UINT)
 * return: NORMAL
 * reply : ["state": 0, ...] OR []
 */
static int aic_cmd_appinfo(struct queue_entry *q, struct cache *cd,
						   mdb_conn *db)
{
	unsigned char *val = NULL;
	size_t vsize = 0;
	int aid, state, hit, ret;
	char *aname, *masn, *email;

	REQ_GET_PARAM_INT(q->hdfrcv, "aid", aid);
	hit = cache_getf(cd, &val, &vsize, PREFIX_AIC"%d", aid);
	if (hit == 0) {
		ret = mdb_exec(db, NULL, "SELECT aid, aname, masn, email, state FROM "
					   " appinfo WHERE aid=%d;", NULL, aid);
		if (ret != MDB_ERR_NONE) {
			mtc_err("exec failure %s", mdb_get_errmsg(db));
			return REP_ERR_DB;
		}
		while (mdb_get(db, "isssi", &aid, &aname, &masn, &email, &state)
			   == MDB_ERR_NONE) {
			hdf_set_int_value(q->hdfsnd, "aid", aid);
			hdf_set_value(q->hdfsnd, "aname", aname);
			hdf_set_value(q->hdfsnd, "masn", masn);
			hdf_set_value(q->hdfsnd, "email", email);
			hdf_set_int_value(q->hdfsnd, "state", state);
		}
		val = calloc(1, MAX_PACKET_LEN);
		if (val == NULL) {
			return REP_ERR_MEM;
		}
		vsize = pack_hdf(q->hdfsnd, val);
		cache_setf(cd, val, vsize, PREFIX_AIC"%d", aid);
		free(val);
	} else {
		unpack_hdf(val, vsize, &q->hdfsnd);
	}
	
	return REP_OK;
}

/*
 * input : aid(UINT) aname(STR) asn(STR) masn(STR) email(STR) state(INT)
 * return: NORMAL REP_ERR_ALREADYREGIST
 * reply : NULL
 */
static int aic_cmd_appnew(struct queue_entry *q, struct cache *cd,
						  mdb_conn *db)
{
	char *aname, *asn, *masn, *email;
	int aid, state, ret;

	REQ_GET_PARAM_INT(q->hdfrcv, "aid", aid);
	REQ_GET_PARAM_INT(q->hdfrcv, "state", state);
	REQ_GET_PARAM_STR(q->hdfrcv, "aname", aname);
	REQ_GET_PARAM_STR(q->hdfrcv, "asn", asn);
	REQ_GET_PARAM_STR(q->hdfrcv, "masn", masn);
	REQ_GET_PARAM_STR(q->hdfrcv, "email", email);

	ret = aic_cmd_appinfo(q, cd, db);
	if (PROCESS_NOK(ret)) {
		mtc_err("info get failure %s", aname);
		return ret;
	}

	if (hdf_get_obj(q->hdfsnd, "state")) {
		mtc_warn("%s already regist", aname);
		return REP_ERR_ALREADYREGIST;
	}

	ret = mdb_exec(db, NULL, "INSERT INTO appinfo (aid, aname, "
				   " asn, masn, email, state) "
				   " VALUES (%d, '%s', '%s', '%s', '%s', %d);",
				   NULL, aid, aname, asn, masn, email, state);
	if (ret != MDB_ERR_NONE) {
		mtc_err("exec failure %s", mdb_get_errmsg(db));
		return REP_ERR_DB;
	}
	
	cache_delf(cd, PREFIX_AIC"%d", aid);

	return REP_OK;
}

/*
 * input : aid(UINT) [aname(STR) asn(STR) masn(STR) email(STR) state(INT)]
 * return: NORMAL REP_ERR_ALREADYREGIST
 * reply : NULL
 */
static int aic_cmd_appup(struct queue_entry *q, struct cache *cd,
						 mdb_conn *db)
{
	char *aname, *asn, *masn, *email;
	int aid, state = -1, ret;

	REQ_GET_PARAM_INT(q->hdfrcv, "aid", aid);
	REQ_FETCH_PARAM_STR(q->hdfrcv, "aname", aname);
	REQ_FETCH_PARAM_STR(q->hdfrcv, "asn", asn);
	REQ_FETCH_PARAM_STR(q->hdfrcv, "masn", masn);
	REQ_FETCH_PARAM_STR(q->hdfrcv, "email", email);
	REQ_FETCH_PARAM_INT(q->hdfrcv, "state", state);

	ret = aic_cmd_appinfo(q, cd, db);
	if (PROCESS_NOK(ret)) {
		mtc_err("info get failure %d", aid);
		return ret;
	}

	if (!hdf_get_obj(q->hdfsnd, "state")) {
		mtc_warn("%d hasn't regist", aid);
		return REP_ERR_NREGIST;
	}

	char cols[1024], tok[128];
	strcpy(cols, "");
	if (aname) {
		snprintf(tok, sizeof(tok), " aname='%s", aname);
		strcat(cols, tok);
		/* avoid strlen(aname) > sizeof(tok) */
		strcat(cols, "', ");
	}
	if (asn) {
		snprintf(tok, sizeof(tok), " asn='%s", asn);
		strcat(cols, tok); strcat(cols, "', ");
	}
	if (masn) {
		snprintf(tok, sizeof(tok), " masn='%s", masn);
		strcat(cols, tok); strcat(cols, "', ");
	}
	if (email) {
		snprintf(tok, sizeof(tok), " email='%s", email);
		strcat(cols, tok); strcat(cols, "', ");
	}
	if (state != -1) {
		snprintf(tok, sizeof(tok), " state=%d, ", state);
		strcat(cols, tok);
	}

	if (!strcmp(cols, "")) {
		return REP_ERR_BADPARAM;
	}
	
	ret = mdb_exec(db, NULL, "UPDATE appinfo SET %s WHERE aid=%d;", NULL, cols, aid);
	if (ret != MDB_ERR_NONE) {
		mtc_err("exec failure %s", mdb_get_errmsg(db));
		return REP_ERR_DB;
	}
	
	cache_delf(cd, PREFIX_AIC"%d", aid);

	return REP_OK;
}

static void aic_process_driver(struct event_entry *entry, struct queue_entry *q)
{
	struct aic_entry *e = (struct aic_entry*)entry;
	int ret = REP_OK;
	
	mdb_conn *db = e->db;
	struct cache *cd = e->cd;
	struct aic_stats *st = &(e->st);

	st->msg_total++;
	
	mtc_dbg("process cmd %u", q->operation);
	switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, cd, ret);
	case REQ_CMD_APPINFO:
		ret = aic_cmd_appinfo(q, cd, db);
		break;
	case REQ_CMD_APPNEW:
		ret = aic_cmd_appnew(q, cd, db);
		break;
	case REQ_CMD_APPUP:
		ret = aic_cmd_appup(q, cd, db);
		break;
	case REQ_CMD_STATS:
		st->msg_stats++;
		ret = REP_OK;
		hdf_set_int_value(q->hdfsnd, "msg_total", st->msg_total);
		hdf_set_int_value(q->hdfsnd, "msg_unrec", st->msg_unrec);
		hdf_set_int_value(q->hdfsnd, "msg_badparam", st->msg_badparam);
		hdf_set_int_value(q->hdfsnd, "msg_stats", st->msg_stats);
		hdf_set_int_value(q->hdfsnd, "proc_suc", st->proc_suc);
		hdf_set_int_value(q->hdfsnd, "proc_fai", st->proc_fai);
		break;
	default:
		st->msg_unrec++;
		ret = REP_ERR_UNKREQ;
		break;
	}
	if (PROCESS_OK(ret)) {
		st->proc_suc++;
	} else {
		st->proc_fai++;
        if (ret == REP_ERR_BADPARAM) {
            st->msg_badparam++;
        }
		mtc_err("process %u failed %d", q->operation, ret);
	}
	if (q->req->flags & FLAGS_SYNC) {
		reply_trigger(q, ret);
	}
}

static void aic_stop_driver(struct event_entry *entry)
{
	struct aic_entry *e = (struct aic_entry*)entry;

	/*
	 * e->base.name, e->base will free by mevent_stop_driver() 
	 */
	mdb_destroy(e->db);
	cache_free(e->cd);
}



static struct event_entry* aic_init_driver(void)
{
	struct aic_entry *e = calloc(1, sizeof(struct aic_entry));
	if (e == NULL) return NULL;

	e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
	e->base.ksize = strlen(PLUGIN_NAME);
	e->base.process_driver = aic_process_driver;
	e->base.stop_driver = aic_stop_driver;

	char *dbsn = hdf_get_value(g_cfg, CONFIG_PATH".dbsn", NULL);
	if (mdb_init(&e->db, dbsn) != RET_RBTOP_OK) {
		wlog("init %s failure %s\n", dbsn, mdb_get_errmsg(e->db));
		goto error;
	} else {
		mtc_info("init %s ok", dbsn);
	}
	
	e->cd = cache_create(hdf_get_int_value(g_cfg, CONFIG_PATH".numobjs", 1024), 0);
	if (e->cd == NULL) {
		wlog("init cache failure");
		goto error;
	}
	
	return (struct event_entry*)e;
	
error:
	if (e->base.name) free(e->base.name);
	if (e->db) mdb_destroy(e->db);
	if (e->cd) cache_free(e->cd);
	free(e);
	return NULL;
}

struct event_driver aic_driver = {
	.name = (unsigned char*)PLUGIN_NAME,
	.init_driver = aic_init_driver,
};
