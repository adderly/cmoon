#ifndef __OMEMBER_H__
#define __OMEMBER_H__
#include "mheads.h"

__BEGIN_DECLS

#define _COL_MEMBER "mname, msn, mmsn, statu, verify, " \
    " phone, contact, "                                 \
    " to_char(intime, 'YYYY-MM-DD') as intime"

enum {
    MEMBER_VF_FRESH = 0,
    MEMBER_VF_ADMIN = 90,
    MEMBER_VF_ROOT
};

/*
 * Description: 验证用户登录（内部使用，非接口）
 * Request: mname - 用户名，query或cookie自带均可
 *          mmsn - 用户临时口令，由member/login生成，query或cookie自带均可
 * Return: {errcode: 25, errmsg: '请登录后操作'}
 */
#define MEMBER_CHECK_LOGIN()                                            \
    do {                                                                \
        err = member_check_login_data_get(cgi, dbh, evth, ses);         \
        if (err != STATUS_OK) return nerr_pass(err);                    \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_RESERVE".mname", mname);        \
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
        if (hdf_get_int_value(cgi->hdf, PRE_OUTPUT".member.verify", -1) < MEMBER_VF_ADMIN) \
            return nerr_raise(LERR_LIMIT, "%s not admin", mname);       \
    } while (0)

NEOERR* member_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* member_logout_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* member_check_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
/*
 * Description: 用户登录
 * Request: mname* - 用户名
 *          msn* - 用户密码
 * Returns: {errcode: 26, errmsg: '密码错误'}
 *          {"mname": "imtestt@163.com", "mmsn": "AOMXKA...", "success": "1" }
 *          其中，mmsn 为用户的临时token，一周内有效，请保存该返回，check_login时会用到
 */
NEOERR* member_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OMEMBER_H__ */
