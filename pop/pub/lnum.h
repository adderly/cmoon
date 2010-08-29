#ifndef __LNUM_H__
#define __LNUM_H__

#include "mheads.h"

__BEGIN_DECLS

enum {
    UIPLUG_TYPE_MOLE = 0,
    UIPLUG_TYPE_ATOM
} uiplug_type;

enum {
	RET_RBTOP_NOTLOGIN = 2000,
	RET_RBTOP_LOGINPSW,
	RET_RBTOP_LIMITE,
	RET_RBTOP_NEEDVIP,
	RET_RBTOP_NEEDVVIP,
	RET_RBTOP_NEXIST = 2010,
	RET_RBTOP_EXISTE,
	RET_RBTOP_IMGPROE = 2020,
	RET_RBTOP_OPENFILE
} ret_rbtop;

#define LMT_CLI_REGIST	400
#define LMT_CLI_LOGIN	400

#define PERIOD_CLI_REGIST	ONE_HOUR
#define PERIOD_CLI_LOGIN	ONE_HOUR

#define LMT_CLI_ATTACK		100
#define PERIOD_CLI_ATTACK	ONE_MINUTE

#define MAX_URI_ITEM	100
#define MAX_GROUP_AUSER	100

__END_DECLS
#endif	/* __LNUM_H__ */
