#ifndef __MOC_PRIVATE_H__
#define __MOC_PRIVATE_H__

#include "lerr.h"
#include "packet.h"
#include "cache.h"

__BEGIN_DECLS

extern volatile time_t g_ctime;

#define MAX_MEMPACK_LEN (6*1024*1024)
#define MAX_PACKET_LEN  (64*1024)
#define RESERVE_SIZE    512

/* Protocol version, for checking in the network header. */
#define PROTO_VER 1

/* Possible request flags (which can be applied to the documented requests) */
#define FLAGS_NONE       0
#define FLAGS_CACHE_ONLY 1    /* get, set, del, cas, incr */
#define FLAGS_SYNC       2    /* set, del */

/* ok start point */
enum {REP_OK = 1000};
#define PROCESS_OK(ret)  (ret >= REP_OK)
#define PROCESS_NOK(ret) (ret < REP_OK)

__END_DECLS
#endif    /* __MOC_PRIVATE_H__ */
