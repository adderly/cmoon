#ifndef __LNUM_H__
#define __LNUM_H__

#include "mheads.h"

__BEGIN_DECLS

enum user_status
{
	USER_FRESH = 0,
	USER_RLSED,
	USER_CFMED
};

#define MIN_USER_NUM	998

#define LMT_CLI_REGIST	400
#define LMT_CLI_LOGIN	400

#define PERIOD_CLI_REGIST	ONE_HOUR
#define PERIOD_CLI_LOGIN	ONE_HOUR

#define LMT_CLI_ATTACK		100
#define PERIOD_CLI_ATTACK	ONE_MINUTE

#define DIV_USER_TB		100

#define MAX_URI_ITEM	100
#define MAX_GROUP_AUSER	100

#define RET_RBTOP_LOGINUSR	2001
#define RET_RBTOP_LOGINPSW	2002
#define RET_RBTOP_RELEASED	2003

#define RET_RBTOP_GETLISTE	2010

#define RET_RBTOP_MMCERR	2020

#define RET_RBTOP_NOTLOGIN	2030
#define RET_RBTOP_LIMITE	2031

#define RET_RBTOP_NEXIST	2040
#define RET_RBTOP_EXISTE	2041

__END_DECLS
#endif	/* __LNUM_H__ */
