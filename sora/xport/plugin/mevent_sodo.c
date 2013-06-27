#include "mevent_plugin.h"
#include "mevent_sodo.h"
#include "sodo_pri.h"
#include "mevent_member.h"

static NEOERR* cmd_point_get(struct sodo_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    struct cache *cd = e->cd;
    mmg_conn *db = e->db;
    int mid, npp, nst;
    NEOERR *err;

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);

    mdb_pagediv(q->hdfrcv, NULL, &npp, &nst, "page", q->hdfsnd);

    if (cache_getf(cd, &val, &vsize, PREFIX_POINT"%d_%d", mid, nst)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MMG_SET_NTT(q->hdfsnd, "page._ntt", db, "sora", "tpoint", "{'mid': %d}", mid);
        
        err = mmg_preparef(db, MMG_FLAG_EMPTY, nst, npp, NULL, NULL,
                           "{'mid': %d}", mid);
        if (err != STATUS_OK) return nerr_pass(err);
        
        err = mmg_query(db, "sora.tpoint", "points", q->hdfsnd);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, POINT_CC_SEC, PREFIX_POINT"%d_%d", mid, nst);
    }

    return STATUS_OK;
}

static NEOERR* cmd_point_add(struct sodo_entry *e, QueueEntry *q)
{
    mmg_conn *db = e->db;
    char *gpos;
    int mid;
    NEOERR *err;

    REQ_GET_PARAM_STR(q->hdfrcv, "p", gpos);

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);

    err = mmg_string_insertf(db, "sora.tpoint",
                             "{'mid': %d, 'gpos': '%s', 'intime': %ld}",
                             mid, gpos, time(NULL));
    if (err != STATUS_OK) return nerr_pass(err);

    cache_delf(e->cd, PREFIX_POINT"%d_0", mid);
    
    return STATUS_OK;
}

static void sodo_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct sodo_entry *e = (struct sodo_entry*)entry;
    NEOERR *err = NULL;
    int ret;
    
    struct sodo_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_SODO_POINT_GET:
        err = cmd_point_get(e, q);
        break;
    case REQ_CMD_SODO_POINT_ADD:
        err = cmd_point_add(e, q);
        break;
    case REQ_CMD_STATS:
        st->msg_stats++;
        err = STATUS_OK;
        hdf_set_int_value(q->hdfsnd, "msg_total", st->msg_total);
        hdf_set_int_value(q->hdfsnd, "msg_unrec", st->msg_unrec);
        hdf_set_int_value(q->hdfsnd, "msg_badparam", st->msg_badparam);
        hdf_set_int_value(q->hdfsnd, "msg_stats", st->msg_stats);
        hdf_set_int_value(q->hdfsnd, "proc_suc", st->proc_suc);
        hdf_set_int_value(q->hdfsnd, "proc_fai", st->proc_fai);
        break;
    default:
        st->msg_unrec++;
        err = nerr_raise(REP_ERR_UNKREQ, "unknown command %u", q->operation);
        break;
    }
    
    NEOERR *neede = mcs_err_valid(err);
    ret = neede ? neede->error : REP_OK;
    if (PROCESS_OK(ret)) {
        st->proc_suc++;
    } else {
        st->proc_fai++;
        if (ret == REP_ERR_BADPARAM) {
            st->msg_badparam++;
        }
        TRACE_ERR(q, ret, err);
    }
    if (q->req->flags & FLAGS_SYNC) {
            reply_trigger(q, ret);
    }
}

static void sodo_stop_driver(EventEntry *entry)
{
    struct sodo_entry *e = (struct sodo_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mmg_destroy(e->db);
    cache_free(e->cd);
}



static EventEntry* sodo_init_driver(void)
{
    struct sodo_entry *e = calloc(1, sizeof(struct sodo_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = sodo_process_driver;
    e->base.stop_driver = sodo_stop_driver;
    //mevent_add_timer(&e->base.timers, 60, true, hint_timer_up_term);

    char *s = hdf_get_value(g_cfg, CONFIG_PATH".mgdb.host", NULL);
    int port = hdf_get_int_value(g_cfg, CONFIG_PATH".mgdb.port", -1);
    int ms = hdf_get_int_value(g_cfg, CONFIG_PATH".mgdb.timeout", 0);
    err = mmg_init(s, port, ms, &e->db);
    JUMP_NOK(err, error);
    
    e->cd = cache_create(hdf_get_int_value(g_cfg, CONFIG_PATH".numobjs", 1024), 0);
    if (e->cd == NULL) {
        wlog("init cache failure");
        goto error;
    }
    
    return (EventEntry*)e;
    
error:
    if (e->base.name) free(e->base.name);
    if (e->db) mmg_destroy(e->db);
    if (e->cd) cache_free(e->cd);
    free(e);
    return NULL;
}

struct event_driver sodo_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = sodo_init_driver,
};
