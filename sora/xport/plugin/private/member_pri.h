#ifndef __MEMBER_PRI_H__
#define __MEMBER_PRI_H__

#define PLUGIN_NAME    "member"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

#define PREFIX_MEMBER   "Member"

#define MEMBER_CC_SEC   60

enum {
    MEMBER_ST_FRESH = 0,
    MEMBER_ST_SPD_OK,
    MEMBER_ST_PAUSE,
    MEMBER_ST_DELETE,
    MEMBER_ST_SPD_FRESH = 10
};

struct member_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct member_entry {
    EventEntry base;
    mmg_conn *db;
    Cache *cd;
    struct member_stats st;
};

#endif  /* __MEMBER_PRI_H__ */
