#include "mevent_plugin.h"
#include "mevent_member.h"
#include "member_pri.h"

static NEOERR* cmd_mem_get(struct member_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    struct cache *cd = e->cd;
    mmg_conn *db = e->db;
    int mid, secure = 0;
    NEOERR *err;

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "_secure", secure);

    if (cache_getf(cd, &val, &vsize, PREFIX_MEMBER"%d", mid)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        if (secure) {
            err = mmg_preparef(db, MMG_FLAG_Z, 0, 1, NULL, NULL,
                               "{'mid': %d}", mid);
        } else {
            err = mmg_preparef(db, MMG_FLAG_Z, 0, 1, NULL,
                               "{'msn': 0, 'mmsn': 0}",
                               "{'mid': %d}", mid);
        }
        if (err != STATUS_OK) return nerr_pass(err);

        err = mmg_query(db, "sora.member", NULL, q->hdfsnd);
        if (nerr_handle(&err, NERR_NOT_FOUND))
            return nerr_raise(REP_ERR_MEMBER_NEXIST, "用户 %d 不存在", mid);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, MEMBER_CC_SEC, PREFIX_MEMBER"%d", mid);
    }

    return STATUS_OK;
}

static NEOERR* cmd_mem_check_mmsn(struct member_entry *e, QueueEntry *q)
{
    char *mmsn, *mmsndb;
    NEOERR *err;

    REQ_GET_PARAM_STR(q->hdfrcv, "mmsn", mmsn);

    hdf_set_value(q->hdfrcv, "_secure", "1");
    
    err = cmd_mem_get(e, q);
    if (err != STATUS_OK) return nerr_pass(err);

    mmsndb = hdf_get_value(q->hdfsnd, "mmsn", NULL);
    if (!mmsndb || strcmp(mmsn, mmsndb))
        err = nerr_raise(REP_ERR_NOTLOGIN, "mmsn incoreect");

    hdf_remove_tree(q->hdfsnd, "mmsn");
    hdf_remove_tree(q->hdfsnd, "msn");
    
    return err;
}

static NEOERR* cmd_mem_check_msn(struct member_entry *e, QueueEntry *q)
{
    char *msn, *msndb;
    NEOERR *err;

    REQ_GET_PARAM_STR(q->hdfrcv, "msn", msn);
    
    hdf_set_value(q->hdfrcv, "_secure", "1");
    
    err = cmd_mem_get(e, q);
    if (err != STATUS_OK) return nerr_pass(err);

    msndb = hdf_get_value(q->hdfsnd, "msn", NULL);
    if (!msndb || strcmp(msn, msndb))
        err = nerr_raise(REP_ERR_NOTLOGIN, "msn incoreect");
    
    hdf_remove_tree(q->hdfsnd, "mmsn");
    hdf_remove_tree(q->hdfsnd, "msn");
    
    return err;
}

static NEOERR* cmd_mem_add(struct member_entry *e, QueueEntry *q)
{
    char *mname;
    NEOERR *err;

    err = cmd_mem_get(e, q);
    nerr_handle(&err, REP_ERR_MEMBER_NEXIST);
    if (err != STATUS_OK) return nerr_pass(err);

    if (hdf_get_obj(q->hdfsnd, "mname"))
        return nerr_raise(REP_ERR_MEMBERED, "%s 已注册", mname);

    err = mmg_hdf_insertl(e->db, "sora.member", q->hdfrcv,
                           hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.member"));
    if (err != STATUS_OK) return nerr_pass(err);

    hdf_set_value(q->hdfsnd, "mid", hdf_get_value(q->hdfrcv, "mid", NULL));

    return STATUS_OK;
}

static NEOERR* cmd_mem_up(struct member_entry *e, QueueEntry *q)
{
    int mid;
    NEOERR *err;

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);

    err = cmd_mem_get(e, q);
    if (err != STATUS_OK) return nerr_pass(err);

    err = mmg_hdf_updatefl(e->db, "sora.member", MMG_FLAG_UPSET, q->hdfrcv,
                           hdf_get_obj(g_cfg, CONFIG_PATH".UpdateCol.member"),
                           "{'mid': %d}", mid);
    if (err != STATUS_OK) return nerr_pass(err);

    cache_delf(e->cd, PREFIX_MEMBER"%d", mid);
    
    return STATUS_OK;
}

static NEOERR* cmd_mem_getrlink(struct member_entry *e, QueueEntry *q)
{
    return STATUS_OK;
}

static NEOERR* cmd_mem_setrlink(struct member_entry *e, QueueEntry *q)
{
    return STATUS_OK;
}

static void member_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct member_entry *e = (struct member_entry*)entry;
    NEOERR *err;
    int ret;
    
    struct member_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_MEMBER_GET:
        err = cmd_mem_get(e, q);
        break;
    case REQ_CMD_MEMBER_CHECK_MMSN:
        err = cmd_mem_check_mmsn(e, q);
        break;
    case REQ_CMD_MEMBER_CHECK_MSN:
        err = cmd_mem_check_msn(e, q);
        break;
    case REQ_CMD_MEMBER_ADD:
        err = cmd_mem_add(e, q);
        break;
    case REQ_CMD_MEMBER_UP:
        err = cmd_mem_up(e, q);
        break;
    case REQ_CMD_MEMBER_GETRLINK:
        err = cmd_mem_getrlink(e, q);
        break;
    case REQ_CMD_MEMBER_SETRLINK:
        err = cmd_mem_setrlink(e, q);
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

static void member_stop_driver(EventEntry *entry)
{
    struct member_entry *e = (struct member_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mmg_destroy(e->db);
    cache_free(e->cd);
}



static EventEntry* member_init_driver(void)
{
    struct member_entry *e = calloc(1, sizeof(struct member_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = member_process_driver;
    e->base.stop_driver = member_stop_driver;
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

struct event_driver member_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = member_init_driver,
};
