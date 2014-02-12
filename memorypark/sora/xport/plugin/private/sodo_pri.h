#ifndef __SODO_PRI_H__
#define __SODO_PRI_H__

#define PLUGIN_NAME    "sodo"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

#define PREFIX_POINT   "Point"

#define POINT_CC_SEC   60

struct sodo_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct sodo_entry {
    EventEntry base;
    mmg_conn *db;
    Cache *cd;
    struct sodo_stats st;
};

#endif  /* __SODO_PRI_H__ */
