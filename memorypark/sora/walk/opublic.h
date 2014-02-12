#ifndef __OPUBLIC_H__
#define __OPUBLIC_H__
#include "mheads.h"

__BEGIN_DECLS

/*
 * Description: 验证用户登录（内部使用，非接口）
 * Request: mname - 用户名，query或cookie自带均可
 *          mmsn - 用户临时口令，由member/login生成，query或cookie自带均可
 * Return: {errcode: 25, errmsg: '请登录后操作'}
 */
#define MEMBER_CHECK_LOGIN()                                        \
    do {                                                            \
        err = member_check_login_data_get(cgi, dbh, evth, ses);     \
        if (err != STATUS_OK) return nerr_pass(err);                \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_RESERVE".mname", mname);    \
    } while (0)

/*
 * Description: 验证用户登录，且是否为管理员（内部使用，非接口）
 * Request: mname - 用户名，query或cookie自带均可
 *          mmsn - 用户临时口令，由member/login生成，query或cookie自带均可
 * Return: {errcode: 25, errmsg: '请登录后操作'}
 *         {errcode: 27, errmsg: '用户无权限'}
 */
#define MEMBER_CHECK_ADMIN()                                            \
    do {                                                                \
        err = member_check_login_data_get(cgi, dbh, evth, ses);         \
        if (err != STATUS_OK) return nerr_pass(err);                    \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_RESERVE".mname", mname);        \
        mevent_t *_evt = hash_lookup(evth, "member");                   \
        MCS_NOT_NULLA(_evt);                                            \
        if (hdf_get_int_value(_evt->hdfrcv, "verify", -1) < MEMBER_VF_ADMIN) \
            return nerr_raise(LERR_LIMIT, "%s not admin", mname);       \
    } while (0)

/*
 * Description: 验证用户登录，且是否为老爸（内部使用，非接口）
 * Request: mname - 用户名，query或cookie自带均可
 *          mmsn - 用户临时口令，由member/login生成，query或cookie自带均可
 * Return: {errcode: 25, errmsg: '请登录后操作'}
 *         {errcode: 27, errmsg: '用户无权限'}
 */
#define MEMBER_CHECK_ROOT()                                             \
    do {                                                                \
        err = member_check_login_data_get(cgi, dbh, evth, ses);         \
        if (err != STATUS_OK) return nerr_pass(err);                    \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_RESERVE".mname", mname);        \
        mevent_t *_evt = hash_lookup(evth, "member");                   \
        MCS_NOT_NULLA(_evt);                                            \
        if (hdf_get_int_value(_evt->hdfrcv, "verify", -1) < MEMBER_VF_ROOT) \
            return nerr_raise(LERR_LIMIT, "%s not admin", mname);       \
    } while (0)

/*
 * called after CHECK_XXX() immediately
 */
#define SET_DASHBOARD_ACTION(out)                                       \
    do {                                                                \
        hdf_set_value(out, PRE_WALK_SACTION".0", "actions_1");          \
        mevent_t *_evt = hash_lookup(evth, "member");                   \
        if (hdf_get_int_value(_evt->hdfrcv, "verify", -1) >= MEMBER_VF_ADMIN) \
            hdf_set_value(out, PRE_WALK_SACTION".1", "actions_2");      \
    }                                                                   \
    while (0)

/* TODO */
#if 0
#define LEGAL_CHECK_NICK(mnick)                                     \
    do {                                                            \
        if (!reg_search("^([a-zA-Z0-9_\.\-])+$", mnick)) {          \
            return nerr_raise(LERR_USERINPUT, "%s illegal", mnick); \
        }                                                           \
    } while(0)

#define LEGAL_CHECK_MNAME(mname)                                        \
    do {                                                                \
        if (!reg_search("^([a-zA-Z0-9_\.\-])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$", mname)) { \
            return nerr_raise(LERR_USERINPUT, "%s illegal", mname);     \
        }                                                               \
    } while(0)
#else
#define LEGAL_CHECK_NICK(mnick)
#define LEGAL_CHECK_NAME(mname)
#endif


__END_DECLS
#endif  /* __OPUBLIC_H__ */
