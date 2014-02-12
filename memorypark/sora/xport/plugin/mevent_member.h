#ifndef __MEVENT_MEMBER_H__
#define __MEVENT_MEMBER_H__

#include "soram.h"

enum {
    REQ_CMD_MEMBER_GET = 1001,
    REQ_CMD_MEMBER_CHECK_MMSN,
    REQ_CMD_MEMBER_CHECK_MSN,
    REQ_CMD_MEMBER_ADD,
    REQ_CMD_MEMBER_UP,
    REQ_CMD_MEMBER_GETRLINK,
    REQ_CMD_MEMBER_SETRLINK,
};

/* verify */
enum {
    MEMBER_VF_FRESH = 0,
    MEMBER_VF_ID,
    MEMBER_VF_DRIVE,
    MEMBER_VF_ADMIN = 90,
    MEMBER_VF_ROOT = 100
};

#define MEMBER_GET_PARAM_MID(hdf, mid)                  \
    do {                                                \
        if (!hdf_get_value(hdf, "mid", NULL)) {         \
            char *_mname = NULL;                        \
            REQ_GET_PARAM_STR(hdf, "mname", _mname);    \
            mid = hash_string_rev(_mname);              \
            hdf_set_int_value(hdf, "mid", mid);         \
        } else mid = hdf_get_int_value(hdf, "mid", 0);  \
    } while (0)

#define MEMBER_SET_PARAM_MID(hdf)                                   \
    do {                                                            \
        if (!hdf_get_value(hdf, "mid", NULL)) {                     \
            char *_mname = NULL;                                    \
            REQ_GET_PARAM_STR(hdf, "mname", _mname);                \
            hdf_set_int_value(hdf, "mid", hash_string_rev(_mname)); \
        }                                                           \
    } while (0)

#endif    /* __MEVENT_MEMBER_H__ */
