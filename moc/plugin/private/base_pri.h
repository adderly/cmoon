#ifndef __BASE_PRI_H__
#define __BASE_PRI_H__

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
    char *uid;
    char ip[INET6_ADDRSTRLEN];
    int port;
    int fd;
    struct tcp_socket *tcpsock;        /* 程序主动挂掉客户端链接时，需要手动设置tcpsock信息 */
};

struct base_info {
    int usernum;
    HASH *userh;
};

NEOERR* base_info_init();
struct base_user *base_user_find(char *uid);
struct base_user *base_user_new(char *uid, QueueEntry *q);
bool base_user_quit(char *uid);
void base_user_destroy(void *arg);

#endif  /* __BASE_PRI_H__ */
