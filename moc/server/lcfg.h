#ifndef __LCFG_H__
#define __LCFG_H__

#define PLUGIN_PATH    "/usr/local/lib/"

enum {
    REQ_CMD_NONE = 0,
    REQ_CMD_CACHE_GET = 100,
    REQ_CMD_CACHE_SET,
    REQ_CMD_CACHE_DEL,
    REQ_CMD_CACHE_EMPTY,
    REQ_CMD_STATS = 1000        /* MAX system command is 1000 */
};

#endif    /* __LCFG_H__ */
