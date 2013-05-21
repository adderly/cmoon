#ifndef __MOC_BASE_H__
#define __MOC_BASE_H__

#define PREFIX_BASE     "Base"

enum {
    REQ_CMD_BASE_JOIN = 1001,
    REQ_CMD_BASE_QUIT
};

struct base_info {
    int usernum;
    HASH *userh;
};
typedef struct base_info BaseInfo;

struct base_user {
    char *uid;
    char ip[INET6_ADDRSTRLEN];
    int port;
    int fd;
    struct tcp_socket *tcpsock;        /* 程序主动挂掉客户端链接时，需要手动设置tcpsock信息 */
    struct base_info *baseinfo;
};
typedef struct base_user BaseUser;


#define CASE_BASE_CMD(binfo, q)                 \
    {                                           \
    case REQ_CMD_BASE_JOIN:                     \
        err = base_cmd_join(binfo, q);          \
        break;                                  \
    case REQ_CMD_BASE_QUIT:                     \
        err = base_cmd_quit(binfo, q);          \
        break;                                  \
    }

#define BASE_GET_UID(q, uid)                                    \
    do {                                                        \
        if (q->req->tcpsock && q->req->tcpsock->appdata) {      \
            uid = ((BaseUser*)q->req->tcpsock->appdata)->uid;   \
        } else {                                                \
            return nerr_raise(REP_ERR_BADPARAM, "请先登陆");    \
        }                                                       \
    } while (0)

#define BASE_FETCH_UID(q, uid)                                  \
    do {                                                        \
        if (q->req->tcpsock && q->req->tcpsock->appdata) {      \
            uid = ((BaseUser*)q->req->tcpsock->appdata)->uid;   \
        } else {                                                \
            uid = NULL;                                         \
        }                                                       \
    } while (0)

NEOERR* base_info_init(BaseInfo **binfo);
struct base_user *base_user_find(BaseInfo *binfo, char *uid);
struct base_user *base_user_new(BaseInfo *binfo, char *uid, QueueEntry *q);
bool base_user_quit(BaseInfo *binfo, char *uid);
void base_user_destroy(void *arg);

NEOERR* base_cmd_join(struct base_info *binfo, QueueEntry *q);
NEOERR* base_cmd_quit(struct base_info *binfo, QueueEntry *q);

#endif    /* __MOC_BASE_H__ */
