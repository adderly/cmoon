#ifndef __OSYSTEM_H__
#define __OSYSTEM_H__
#include "mheads.h"

/*
 * system 管理员操作
 */

__BEGIN_DECLS

/*
 * Description: 获取访问量，用户量统计数据
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of member.h
 * Returns: None
 */
NEOERR* system_view_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 查看某段时间内访问的用户
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of member.h
 *          hour* - YYYY-MM-DD H 时段内的用户
 * Returns: None
 */
NEOERR* system_who_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 查看某个用户的浏览记录
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of member.h
 *          sender* - 要查看的用户名
 *          day* - 某天的活动
 * Returns: None
 */
NEOERR* system_view_detail_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OSYSTEM_H__ */
