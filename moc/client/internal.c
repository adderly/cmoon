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
