#ifndef __OZUP_H__
#define __OZUP_H__
#include "mheads.h"
#include "omember.h"

__BEGIN_DECLS

/*
 * Description: 获取用户坐标轨迹
 * Request: LOGIN* - 用户登陆,请参考 MEMBER_CHECK_LOGIN() of opublic.h
 *          _npp - number perpage 每页显示条目
 *          _npg - page number 分页页码
 * Returns: member, page, points...
 */
NEOERR* zup_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 添加用户轨迹
 * Request: LOGIN* - 用户登陆,请参考 MEMBER_CHECK_LOGIN() of opublic.h
 *          p* - geo point 经纬度坐标
 * Returns: {"success": "1"}
 */
NEOERR* zup_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OZUP_H__ */
