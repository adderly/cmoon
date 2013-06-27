#ifndef __SORAM_H__
#define __SORAM_H__

enum {
    REP_ERR_NOTLOGIN = 25,      /* for member_check_mmsn */
    REP_ERR_LOGINPSW = 26,      /* for member_check_msn */
    REP_ERR_MEMBERED = 31,
    REP_ERR_CARED,
    REP_ERR_MEMBER_NEXIST
};

#endif  /* __SORAM_H__ */
