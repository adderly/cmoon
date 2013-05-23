#include "moc.h"

/*
 * Hash function used internally by select_srv(). See RFC 1071.
 */
static uint32_t checksum(const unsigned char *buf, size_t bsize)
{
    uint32_t sum = 0;

    while (bsize > 1)  {
        sum += * (uint16_t *) buf++;
        bsize -= 2;
    }

    if (bsize > 0)
        sum += * (uint8_t *) buf;

    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

#ifdef EVENTLOOP
static void threadsync_create(struct threadsync *sync)
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&sync->lock, &attr);
    pthread_mutexattr_destroy(&attr);

    pthread_cond_init(&sync->cond, NULL);
}
#endif

moc_arg* mocarg_init()
{
    moc_arg *arg = calloc(1, sizeof(moc_arg));
    if (!arg) return NULL;

    hash_init(&arg->evth, hash_str_hash, hash_str_comp);

#ifdef EVENTLOOP
    threadsync_create(&(arg->mainsync));
    threadsync_create(&(arg->eloopsync));
    threadsync_create(&(arg->callbacksync));
#endif

    return arg;
}

void mocarg_destroy(moc_arg *arg)
{
    if (!arg) return;

    if (arg->evth) hash_destroy(&arg->evth);

#ifdef EVENTLOOP
    pthread_mutex_destroy(&(arg->mainsync.lock));
    pthread_mutex_destroy(&(arg->eloopsync.lock));
    pthread_mutex_destroy(&(arg->callbacksync.lock));
#endif

    free(arg);
}

/*
 * Like recv(), but either fails, or returns a complete read; if we return
 * less than count is because EOF was reached.
 */
ssize_t srecv(int fd, unsigned char *buf, size_t count, int flags)
{
    ssize_t rv, c;

    c = 0;

    while (c < count) {
        rv = recv(fd, buf + c, count - c, flags);

        if (rv == count)
            return count;
        else if (rv < 0)
            return rv;
        else if (rv == 0)
            return c;

        c += rv;
    }
    return count;
}

/*
 * Like srecv(), but for send().
 */
ssize_t ssend(int fd, const unsigned char *buf, size_t count, int flags)
{
    ssize_t rv, c;

    c = 0;

    while (c < count) {
        rv = send(fd, buf + c, count - c, flags);

        if (rv == count)
            return count;
        else if (rv < 0)
            return rv;
        else if (rv == 0)
            return c;

        c += rv;
    }
    return count;
}

/*
 * Compares two servers by their connection identifiers. It is used internally
 * to keep the server array sorted with qsort().
 */
int compare_servers(const void *s1, const void *s2)
{
    moc_srv *srv1 = (moc_srv*) s1;
    moc_srv *srv2 = (moc_srv*) s2;

    in_addr_t a1, a2;
    a1 = srv1->srvsa.sin_addr.s_addr;
    a2 = srv2->srvsa.sin_addr.s_addr;

    if (a1 < a2) {
        return -1;
    } else if (a1 == a2) {
        in_port_t p1, p2;
        p1 = srv1->srvsa.sin_port;
        p2 = srv2->srvsa.sin_port;

        if (p1 < p2)
            return -1;
        else if (p1 == p2)
            return 0;
        else
            return 1;
    } else {
        return 1;
    }
}

/* Selects which server to use for the given key. */
moc_srv *select_srv(moc_t *evt, const char *key, size_t ksize)
{
    uint32_t n;
    
    if (evt->nservers == 0)
        return NULL;
    
    n = checksum((const unsigned char*)key, ksize) % evt->nservers;
    return &(evt->servers[n]);
}

void close_srv(moc_t *evt, int order, int fd)
{
    if (!evt) return;
    
    mtc_dbg("%s %d %d closed", evt->ename, order, fd);

    if (evt->nservers < order) return;

    moc_srv *srv = &(evt->servers[order]);

    if (srv->fd != fd) return;

    close(srv->fd);
    srv->fd = -1;
}

#ifdef EVENTLOOP
/*
 * application logic message, called by eventloop thread
 */
static NEOERR* parse_message(moc_srv *srv, unsigned char *buf, size_t len, moc_arg *arg)
{
    uint32_t id, reply;
    unsigned char *payload;
    size_t psize, rv;

    MOC_NOT_NULLB(arg, arg->evth);
    
    if (!srv || !buf || len < 8) return nerr_raise(NERR_ASSERT, "illegal packet");

    //MSG_DUMP("recv: ", buf, len);

    /* The header is:
     * 4 bytes    ID
     * 4 bytes    Reply Code
     * Variable   Payload
     */
    id = ntohl(* ((uint32_t *) buf));
    reply = ntohl(* ((uint32_t *) buf + 1));

    payload = buf + 8;
    psize = len - 8;

    if (id == 0 && reply == 10000) {
        /*
         * server push
         */
        if (psize < 4) return nerr_raise(NERR_ASSERT, "server pushed empty message");

        rv = unpack_hdf(payload + 4, psize - 4, &(srv->evt->hdfrcv));
        if (rv <= 0) return nerr_raise(NERR_ASSERT, "server pushed illegal message");

        TRACE_HDF(srv->evt->hdfrcv);

        char *cmd = NULL;
        HDF_ATTR *attr = hdf_get_attr(srv->evt->hdfrcv, "_Reserve");
        while (attr != NULL) {
            if (!strcmp(attr->key, "cmd")) cmd = attr->value;
            attr = attr->next;
        }
        if (!cmd) return nerr_raise(NERR_ASSERT, "cmd not supplied");

        mtc_dbg("receive cmd %s", cmd);
        
        hdf_remove_tree(srv->evt->hdfrcv, "_Reserve");

        /*
         * notify callback thread
         */
        
    } else {
        /*
         * server response
         */
        if (id < g_reqid)
            return nerr_raise(NERR_ASSERT, "id not match %d %d", g_reqid, id);

        if (psize >= 4) {
            pthread_mutex_lock(&(arg->mainsync.lock));
            rv = unpack_hdf(payload + 4, psize - 4, &(srv->evt->hdfrcv));
            pthread_mutex_unlock(&(arg->mainsync.lock));
            if (rv <= 0)
                return nerr_raise(NERR_ASSERT, "server responsed illegal message");

            TRACE_HDF(srv->evt->hdfrcv);
        }

        /*
         * notify main thread
         */
        pthread_cond_signal(&(arg->mainsync.cond));
    }
    
    return STATUS_OK;
}

void process_buf_srv(moc_t *evt, int order, int fd,
                     unsigned char *buf, size_t len, moc_arg *arg)
{
    uint32_t totaltoget = 0;
    moc_srv *srv;
    NEOERR *err;

    if (!evt || !buf) {
        mtc_err("param null");
        return;
    }

    /*
     * validate
     */
    if (evt->nservers < order) {
        mtc_err("order %d > server %d", order, evt->nservers);
        return;
    }
    
    srv = &(evt->servers[order]);
    if (srv->fd != fd) {
        mtc_err("fd not equal %d %d", srv->fd, fd);
        return;
    }
    
    /*
     * copy from process_buf() on server/tcp.c
     */
    if (len >= 4) {
        totaltoget = * (uint32_t*) buf;
        totaltoget = ntohl(totaltoget);
        if (totaltoget > (64 * 1024) || totaltoget <= 8) {
            mtc_err("message illegal %d", totaltoget);
            return;
        }
    } else totaltoget = 4;

    if (totaltoget > len) {
        if (srv->buf == NULL) {
            srv->buf = malloc(SBSIZE);
            if (!srv->buf) {
                mtc_foo("out of memory");
                return;
            }

            memcpy(srv->buf, buf, len);
            srv->len = len;
        } else {
            srv->len = len;
        }

        srv->pktsize = totaltoget;

        /* need rereceive */
        return;
    }

    if (totaltoget < len) {
        srv->excess = len - totaltoget;
        len = totaltoget;
    }

    err = parse_message(srv, buf + 4, len - 4, arg);
    TRACE_NOK(err);

    if (srv->excess) {
        memmove(buf, buf + len, srv->excess);
        srv->len = srv->excess;
        srv->excess = 0;

        process_buf_srv(evt, order, fd, buf, srv->len, arg);
        /* need reprocess */
        return;
    }

    if (srv->buf) {
        free(srv->buf);
        srv->buf = NULL;
        srv->len = 0;
        srv->pktsize = 0;
        srv->excess = 0;
    }

    return;
}
#endif
