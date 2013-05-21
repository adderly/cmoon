#include "moc_plugin.h"
#include "moc_base.h"
#include "base_pri.h"

NEOERR* base_info_init(struct base_info **binfo)
{
    NEOERR *err;

    MCS_NOT_NULLA(binfo);

    if (!*binfo) {
        struct base_info *linfo = calloc(1, sizeof(struct base_info));
        if (!linfo) return nerr_raise(NERR_NOMEM, "alloc base failure");
        linfo->usernum = 0;
        err = hash_init(&linfo->userh, hash_str_hash, hash_str_comp);
        if (err != STATUS_OK) return nerr_pass(err);

        *binfo = linfo;
    }

    return STATUS_OK;
}

struct base_user *base_user_find(struct base_info *binfo, char *uid)
{
    if (!binfo || !uid) return NULL;
    
    return (struct base_user*)hash_lookup(binfo->userh, uid);
}

struct base_user *base_user_new(struct base_info *binfo, char *uid, QueueEntry *q)
{
    if (!binfo || !uid || !q || !q->req) return NULL;
    
    struct base_user *user = calloc(1, sizeof(struct base_user));
    if (!user) return NULL;

    struct sockaddr_in *clisa = (struct sockaddr_in*)q->req->clisa;

    //strncpy(user->ip, inet_ntoa(clisa), 16);
    user->uid = strdup(uid);
    user->fd = q->req->fd;
    inet_ntop(clisa->sin_family, &clisa->sin_addr,
              user->ip, sizeof(user->ip));
    user->port = ntohs(clisa->sin_port);
    user->tcpsock = q->req->tcpsock;
    user->baseinfo = binfo;

    /*
     * used on user close
     */
    if (q->req->tcpsock) {
        q->req->tcpsock->appdata = user;
        q->req->tcpsock->on_close = base_user_destroy;
    }
    
    /*
     * binfo
     */
    hash_insert(binfo->userh, (void*)strdup(uid), (void*)user);
    binfo->usernum++;
    
    mtc_dbg("%s %s %d join", uid, user->ip, user->port);
    
    return user;
}

bool base_user_quit(struct base_info *binfo, char *uid)
{
    struct tcp_socket *tcpsock;
    struct base_user *user;

    user = base_user_find(binfo, uid);
    if (!user) return false;
    
    mtc_dbg("%s %s %d quit", user->uid, user->ip, user->port);

    close(user->fd);

    /*
     * TODO
     * we should do this on main thread.
     * it works fine on test, don't known how works on future
     */
    tcpsock = user->tcpsock;
    if (tcpsock) {
        tcpsock->appdata = NULL;
        tcpsock->on_close = NULL;
        event_del(tcpsock->evt);
        tcp_socket_free(tcpsock);
    }
    
    base_user_destroy(user);
    
    return true;
}

void base_user_destroy(void *arg)
{
    struct base_user *user = (struct base_user*)arg;
    struct base_info *binfo = user->baseinfo;

    if (!user || !binfo) return;
    
    mtc_dbg("%s %s %d destroy", user->uid, user->ip, user->port);

    hash_remove(binfo->userh, user->uid);
    if (binfo->usernum > 0) binfo->usernum--;

    SAFE_FREE(user->uid);
    SAFE_FREE(user);

    return;
}
