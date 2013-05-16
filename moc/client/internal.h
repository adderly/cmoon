#ifndef __INTERNAL_H__
#define __INTERNAL_H__

__BEGIN_DECLS

/*
 * macros
 * ========
 */
#define TCP_MSG_OFFSET 4
#define ID_CODE 1

#define MOC_NOT_NULLA(pa)                                       \
    if (!pa) return nerr_raise(NERR_ASSERT, "paramter null");
#define MOC_NOT_NULLB(pa, pb)                                           \
    if (!pa || !pb) return nerr_raise(NERR_ASSERT, "paramter null");
#define MOC_NOT_NULLC(pa, pb, pc)                                       \
    if (!pa || !pb || !pc) return nerr_raise(NERR_ASSERT, "paramter null");


/*
 * types
 * ========
 */
typedef struct _moc_srv {
    int fd;
    struct sockaddr_in srvsa;
    socklen_t srvlen;
} moc_srv;

typedef struct moc_t {
    unsigned int nservers;
    moc_srv *servers;
    int cmd;
    int flags;
    int errcode;
    int packed;
    char *ename;
    size_t psize;

    unsigned char *rcvbuf;
    unsigned char *payload;

    HDF *hdfrcv;
    HDF *hdfsnd;
} moc_t;

typedef void (*MocCallback)(HDF *hdfrcv);


/*
 * externs
 * ========
 */
extern unsigned int g_reqid;


/*
 * functions
 * ========
 */
int compare_servers(const void *s1, const void *s2);
moc_srv *select_srv(moc_t *evt, const char *key, size_t ksize);
ssize_t srecv(int fd, unsigned char *buf, size_t count, int flags);
ssize_t ssend(int fd, const unsigned char *buf, size_t count, int flags);

__END_DECLS
#endif  /* __INTERNAL_H__ */
