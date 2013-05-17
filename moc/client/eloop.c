#include "moc.h"

#define MOC_MAX_CON 100

struct el_con {
    char *name;
    int order;                  /* used on reconnect */
    int fd;
};

static pthread_t *m_thread = NULL;
static bool m_stop = false;

static void* el_routine(void *arg)
{
    HASH *evth = (HASH*)arg;
    char *key = NULL;

    struct el_con conn[MOC_MAX_CON] = {0};
    int num_conn = 0;
    
    moc_t *evt = hash_next(evth, (void**)&key);
    while (evt) {
        for (int i = 0; i < evt->nservers && num_conn < MOC_MAX_CON; i++) {
            moc_srv *srv = &(evt->servers[i]);
            conn[num_conn].name = strdup(evt->ename);
            conn[num_conn].order = i + 1;
            conn[num_conn].fd = srv->fd;
            
            num_conn++;
        }
        
        evt = hash_next(evth, (void**)&key);
    }

    mtc_dbg("start event loop thread...");

    fd_set readset;
    struct timeval tv;
    for (;;) {
        int maxfd = -1;
        FD_ZERO(&readset);

        for (int i = 0; i < num_conn; i++) {
            if (conn[i].fd > maxfd) maxfd = conn[i].fd;
            FD_SET(conn[i].fd, &readset);
        }

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        select(maxfd + 1, &readset, NULL, NULL, &tv);

        for (int i = 0; i < num_conn; i++) {
            if (FD_ISSET(conn[i].fd, &readset)) {
                mtc_dbg("msg from %d fd %d", conn[i].order, conn[i].fd);
            }
        }

        if (m_stop) break;
    }

    return NULL;
}

NEOERR* eloop_start(HASH *evth)
{
    if (m_thread) return nerr_raise(NERR_ASSERT, "eloop started already");

    m_stop = false;
    m_thread = calloc(1, sizeof(pthread_t));
    pthread_create(m_thread, NULL, el_routine, (void*)evth);
}

void eloop_stop(HASH *evth)
{
    if (!m_thread) return;
    
    mtc_dbg("end event loop thread...");

    m_stop = true;
    pthread_join(*m_thread, NULL);
    free(m_thread);
    m_thread = NULL;
}
