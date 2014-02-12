#ifndef __OMEMBER_H__
#define __OMEMBER_H__
#include "mheads.h"

__BEGIN_DECLS

/*
 * Description: 判断邮箱是否已被注册
 * Request: mname* - 需要判断的邮箱号
 * Returns: {success: "1", exist: '1', msg: "用户已被注册"}
 *          {success: "1", exist: '0', msg: "用户还未注册"}
 */
NEOERR* member_exist_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 注册新用户
 * Request: mname* - 用户邮箱
 *          mnick* - 用户昵称
 *          msn* - 用户密码
 *          phone - 用户电话（可选）
 * Returns: None
 */
NEOERR* member_new_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户登录
 * Request: mname* - 用户邮箱
 *          msn* - 用户密码
 * Returns: {errcode: 26, errmsg: '密码错误'}
 *          {"mname": "imtestt@163.com", "mmsn": "AOMXKA...", "success": "1" }
 *          其中，mmsn 为用户的临时token，一周内有效，请保存该返回，check_login时会用到
 */
NEOERR* member_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户登出
 * Request: LOGIN* - 请参考 MEMBER_CHECK_LOGIN() of opublic.h
 * Returns: None
 */
NEOERR* member_logout_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 判断登录是否有效（根据mname和mmsn）
 * Request: mname* - 用户邮箱，cookie或query中自带均可
 *          mmsn* - 登录时返回的 mmsn，临时token，一周内有效，cooke或query中自带均可
 * Returns: {errcode: 25, errmsg: '请登录后操作'}
 *          登录用户的用户信息写到 cgi->hdf 的 Output.member 中
 */
NEOERR* member_check_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 申请重置用户密码，重置连接将发往用户邮箱
 * Request: mname* - 用户注册时填的邮箱
 * Returns: None
 */
NEOERR* member_reset_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户帐号信息页面（登录用户，或者通过重置邮件跳转）
 * Request: LOGIN - see MEMBER_CHECK_LOGIN() of opublic.h
 *          mname - 用户邮箱
 *          rlink - 重置密码时，系统生成的验证码
 *          (LOGIN，或者mname,rlink 二选一必填)
 * Returns: None
 */
NEOERR* member_account_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OMEMBER_H__ */
