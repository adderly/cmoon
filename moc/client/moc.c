#include "moc.h"

unsigned int g_reqid = 0;
static HASH *m_evth = NULL;

NEOERR* moc_init()
{
    HDF *cfg, *node, *cnode;
    NEOERR *err;

    if (m_evth) return nerr_raise(NERR_ASSERT, "moc inited already");
    
    hdf_init(&cfg);
    err = hdf_read_file(cfg, MOC_CONFIG_FILE);
    if (err != STATUS_OK) return nerr_pass(err);

    mtc_init(hdf_get_value(cfg, "Config.logfile", "/tmp/mocclient"),
             hdf_get_int_value(cfg, "Config.trace_level", TC_DEFAULT_LEVEL));

    lerr_init();

    err = hash_init(&m_evth, hash_str_hash, hash_str_comp);
    if (err != STATUS_OK) return nerr_pass(err);

    node = hdf_get_child(cfg, "modules");
    while (node) {
        /*
         *per backend module
         */
        moc_t *evt;
        char *mname;

        mname = hdf_obj_name(node);

        mtc_dbg("init event %s", mname);
        
        evt = calloc(1, sizeof(moc_t));
        if (!evt) return nerr_raise(NERR_NOMEM, "memory gone");

        evt->ename = strdup(mname);

        hdf_init(&evt->hdfrcv);
        hdf_init(&evt->hdfsnd);
        evt->rcvbuf = calloc(1, MAX_PACKET_LEN);
        evt->payload = calloc(1, MAX_PACKET_LEN);
        if (!evt->payload || !evt->rcvbuf) return nerr_raise(NERR_NOMEM, "memory gone");
        
        cnode = hdf_obj_child(node);
        while (cnode) {
            /*
             * per server
             */
            struct timeval tv;
            char *ip = hdf_get_value(cnode, "ip", "127.0.0.1");
            int port = hdf_get_int_value(cnode, "port", 5000);
            int nblk = hdf_get_int_value(cnode, "non_block", 0);
            tv.tv_sec = hdf_get_int_value(cnode, "timeout_s", 0);
            tv.tv_usec = hdf_get_int_value(cnode, "timeout_u", 0);

            if (moc_add_tcp_server(evt, ip, port, nblk, &tv)) {
                mtc_dbg("%s add server %s %d ok", mname, ip, port);
            } else {
                mtc_dbg("%s add server %s %d failure", mname, ip, port);
            }

            cnode = hdf_obj_next(cnode);
        }

        if (evt->nservers) hash_insert(m_evth, (void*)strdup(mname), (void*)evt);

        node = hdf_obj_next(node);
    }

#ifdef EVENTLOOP
    err = eloop_start(m_evth);
    if (err != STATUS_OK) return nerr_pass(err);
#endif

    hdf_destroy(&cfg);
    return STATUS_OK;
}

void moc_destroy()
{
    char *key = NULL;

    if (!m_evth) return;
    
    moc_t *evt = (moc_t*)hash_next(m_evth, (void**)&key);

    while (evt != NULL) {
        /* TODO moc_free */
        //moc_free(evt);
        evt = hash_next(m_evth, (void**)&key);
    }

#ifdef EVENTLOOP
    eloop_stop(m_evth);
#endif

    hash_destroy(&m_evth);
    m_evth = NULL;
}

HDF* moc_hdfsnd(char *module)
{
    if (!m_evth || !module) return NULL;

    moc_t *evt = hash_lookup(m_evth, module);
    if (!evt) return NULL;

    return evt->hdfsnd;
}

NEOERR* moc_set_param(char *module, char *key, char *val)
{
    MOC_NOT_NULLA(m_evth);
    MOC_NOT_NULLC(module, key, val);

    moc_t *evt = hash_lookup(m_evth, module);
    MOC_NOT_NULLA(evt);

    hdf_set_value(evt->hdfsnd, key, val);

    return STATUS_OK;
}

NEOERR* moc_set_param_int(char *module, char *key, int val)
{
    MOC_NOT_NULLA(m_evth);
    MOC_NOT_NULLC(module, key, val);

    moc_t *evt = hash_lookup(m_evth, module);
    MOC_NOT_NULLA(evt);

    hdf_set_int_value(evt->hdfsnd, key, val);

    return STATUS_OK;
}

NEOERR* moc_set_param_uint(char *module, char *key, unsigned int val)
{
    MOC_NOT_NULLA(m_evth);
    MOC_NOT_NULLC(module, key, val);

    moc_t *evt = hash_lookup(m_evth, module);
    MOC_NOT_NULLA(evt);

    char buf[64];
    snprintf(buf, sizeof(buf), "%u", val);

    hdf_set_value(evt->hdfsnd, key, buf);

    return STATUS_OK;
}

NEOERR* moc_set_param_int64(char *module, char *key, int64_t val)
{
    MOC_NOT_NULLA(m_evth);
    MOC_NOT_NULLC(module, key, val);

    moc_t *evt = hash_lookup(m_evth, module);
    MOC_NOT_NULLA(evt);

    char buf[64];
    snprintf(buf, sizeof(buf), "%ld", val);

    hdf_set_value(evt->hdfsnd, key, buf);

    return STATUS_OK;
}

NEOERR* moc_set_param_float(char *module, char *key, float val)
{
    MOC_NOT_NULLA(m_evth);
    MOC_NOT_NULLC(module, key, val);

    moc_t *evt = hash_lookup(m_evth, module);
    MOC_NOT_NULLA(evt);

    char buf[64];
    snprintf(buf, sizeof(buf), "%f", val);

    hdf_set_value(evt->hdfsnd, key, buf);

    return STATUS_OK;
}

int moc_trigger(char *module, char *key, unsigned short cmd, unsigned short flags)
{
    size_t t, ksize, vsize;
    moc_srv *srv;
    unsigned char *p;
    moc_t *evt;
    uint32_t rv = REP_OK;

    if (!m_evth || !module) return REP_ERR;

    evt = hash_lookup(m_evth, module);
    if (!evt) {
        mtc_err("can't found %s module", module);
        return REP_ERR;
    }

    evt->cmd = cmd;
    evt->flags = flags;
    ksize = strlen(module);

    if (key) {
        srv = select_srv(evt, key, strlen(key));
    } else {
        srv = &(evt->servers[0]);
    }

    if (g_reqid++ > 0x0FFFFFFC) {
        g_reqid = 1;
    }

    p = evt->payload + TCP_MSG_OFFSET;
    * (uint32_t *) p = htonl( (PROTO_VER << 28) | g_reqid );
    * ((uint16_t *) p + 2) = htons(cmd);
    * ((uint16_t *) p + 3) = htons(flags);
    * ((uint32_t *) p + 2) = htonl(ksize);
    memcpy(p+12, evt->ename, ksize);

    evt->psize = TCP_MSG_OFFSET + 12 + ksize;
    
    /*
     * don't escape the hdf because some body need set ' in param 
     */
    vsize = pack_hdf(evt->hdfsnd, evt->payload + evt->psize, MAX_PACKET_LEN);
    evt->psize += vsize;

    if (evt->psize < 17) {
        * (uint32_t *) (evt->payload+evt->psize) = htonl(DATA_TYPE_EOF);
        evt->psize += sizeof(uint32_t);
    }
    
    t = tcp_srv_send(srv, evt->payload, evt->psize);
    if (t <= 0) {
        evt->errcode = REP_ERR_SEND;
        return REP_ERR_SEND;
    }

#ifdef EVENTLOOP
#else
    hdf_destroy(&evt->hdfsnd);
    hdf_init(&evt->hdfsnd);
    hdf_destroy(&evt->hdfrcv);
    hdf_init(&evt->hdfrcv);
    
    if (flags & FLAGS_SYNC) {
        vsize = 0;
        rv = tcp_get_rep(srv, evt->rcvbuf, MAX_PACKET_LEN, &p, &vsize);
        if (rv == -1) rv = REP_ERR;
        evt->errcode = rv;

        if (vsize > 8) {
            /* reply_long add a vsize parameter */
            unpack_hdf(p+4, vsize-4, &evt->hdfrcv);
        }
    }
#endif
    
    return rv;
}

HDF* moc_hdfrcv(char *module)
{
    if (!m_evth || !module) return NULL;

    moc_t *evt = hash_lookup(m_evth, module);
    if (!evt) return NULL;

    return evt->hdfrcv;
}

NEOERR* moc_regist_callback(char *module, unsigned short cmd, MocCallback cmdcbk)
{
#ifdef EVENTLOOP
#else
    mtc_foo("can't regist callback without EVENTLOOP");
#endif
    return STATUS_OK;
}
