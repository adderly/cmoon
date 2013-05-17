#include "moc_plugin.h"
#include "moc_base.h"

#define PLUGIN_NAME    "base"
#define CONFIG_PATH    PRE_SERVER"."PLUGIN_NAME

struct base_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct base_entry {
    EventEntry base;
    mdb_conn *db;
    Cache *cd;
    struct base_stats st;
};

struct base_user {
    char *nick;
    int fd;
};

struct _base {
    int ntt;
    HASH *userh;
};

static struct _base *m_base = NULL;

static NEOERR* base_init()
{
    if (!m_base) {
        m_base = calloc(1, sizeof(struct _base));
        if (!m_base) return nerr_raise(NERR_NOMEM, "alloc base failure");
        m_base->ntt = 0;
        err = hash_init(&m_base->userh, hash_str_hash, hash_str_comp);
        if (err != STATUS_OK) return nerr_pass(err);
    }

    return STATUS_OK;
}

static NEOERR* cmd_join(struct base_entry *e, QueueEntry *q)
{
    char *nick;

    REQ_GET_PARAM_STR(q->hdfrcv, "userid", nick);

    mtc_dbg("%s on", nick);
    
    return STATUS_OK;
}

static NEOERR* cmd_say(struct base_entry *e, QueueEntry *q)
{
    char *nick, *msg;

    REQ_GET_PARAM_STR(q->hdfrcv, "userid", nick);
    REQ_GET_PARAM_STR(q->hdfrcv, "msg", msg);

    mtc_dbg("%s say %s", nick, msg);
    
    return STATUS_OK;
}

static void base_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct base_entry *e = (struct base_entry*)entry;
    NEOERR *err = NULL;
    int ret;
    
    struct base_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_BASE_JOIN:
        err = cmd_join(e, q);
        break;
    case REQ_CMD_BASE_SAY:
        err = cmd_say(e, q);
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

static void base_stop_driver(EventEntry *entry)
{
    struct base_entry *e = (struct base_entry*)entry;

    /*
     * e->base.name, e->base will free by moc_stop_driver() 
     */
    mdb_destroy(e->db);
    cache_free(e->cd);
}



static EventEntry* base_init_driver(void)
{
    struct base_entry *e = calloc(1, sizeof(struct base_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = base_process_driver;
    e->base.stop_driver = base_stop_driver;
    //moc_add_timer(&e->base.timers, 60, true, hint_timer_up_term);

    //char *s = hdf_get_value(g_cfg, CONFIG_PATH".dbsn", NULL);
    //err = mdb_init(&e->db, s);
    //JUMP_NOK(err, error);

    err = base_init();
    JUMP_NOK(err, error);
    
    e->cd = cache_create(hdf_get_int_value(g_cfg, CONFIG_PATH".numobjs", 1024), 0);
    if (e->cd == NULL) {
        mtc_err("init cache failure");
        goto error;
    }
    
    return (EventEntry*)e;
    
error:
    if (e->base.name) free(e->base.name);
    if (e->db) mdb_destroy(e->db);
    if (e->cd) cache_free(e->cd);
    free(e);
    return NULL;
}

struct event_driver base_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = base_init_driver,
};
