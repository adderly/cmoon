#ifndef __MEVENT_AUX_H__
#define __MEVENT_AUX_H__

#define PREFIX_CMTAPP        "Comment"
#define PREFIX_IMP            "Improve"
#define PREFIX_IMP_DETAIL    "ImproveDetail"
#define CMT_CC_SEC            60
#define IMP_CC_SEC            60

enum {
    CMT_ST_NORMAL = 0,
    CMT_ST_DEL
};

enum {
    CMT_TYPE_BLOG = 0,
    CMT_TYPE_IMPROVE
};

enum {
    REQ_CMD_CMT_GET = 1001,
    REQ_CMD_CMT_ADD,
    REQ_CMD_CMT_DEL,
    REQ_CMD_MAIL_ADD = 2001,
    REQ_CMD_IMP_GET = 2011,
    REQ_CMD_IMP_ADD,
    REQ_CMD_IMP_DETAIL
};

#endif    /* __MEVENT_AUX_H__ */
