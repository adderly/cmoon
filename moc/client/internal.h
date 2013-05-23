#ifndef __INTERNAL_H__
#define __INTERNAL_H__

__BEGIN_DECLS

/*
 * macros
 * ========
 */
#define TCP_MSG_OFFSET 4
#define ID_CODE 1
#define MOC_MAX_CON 100
#define SBSIZE (68 * 1024)

#define MOC_NOT_NULLA(pa)                                       \
    if (!pa) return nerr_raise(NERR_ASSERT, "paramter null");
#define MOC_NOT_NULLB(pa, pb)                                           \
    if (!pa || !pb) return nerr_raise(NERR_ASSERT, "paramter null");
#define MOC_NOT_NULLC(pa, pb, pc)                                       \
    if (!pa || !pb || !pc) return nerr_raise(NERR_ASSERT, "paramter null");

#define TRACE_NOK(err)                          \
    if (err != STATUS_OK) {                     \
        STRING zstra;    string_init(&zstra);   \
        nerr_error_traceback(err, &zstra);      \
        mtc_err("%s", zstra.buf);               \
        string_clear(&zstra);                   \
        nerr_ignore(&err);                      \
    }
#define TRACE_HDF(node)                         \
    do {                                        \
        STRING zstra;    string_init(&zstra);   \
        hdf_dump_str(node, NULL, 2, &zstra);    \
        mtc_foo("%s", zstra.buf);               \
        string_clear(&zstra);                   \
    } while (0)


/*
 * types
 * ========
 */
#ifdef EVENTLOOP
struct threadsync {
    pthread_mutex_t lock;
    pthread_cond_t cond;
};
#endif

typedef struct {
    HASH *evth;

#ifdef EVENTLOOP
    struct threadsync mainsync;
    struct threadsync eloopsync;
    struct threadsync callbacksync;
#endif
} moc_arg;

typedef struct _moc_srv {
    int fd;
    struct sockaddr_in srvsa;
    socklen_t srvlen;
    int nblock;
    struct timeval tv;
    struct moc_t *evt;

    /*
     * process server's message
     */
    unsigned char *buf;
    size_t len;
    size_t pktsize;
    size_t excess;
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
moc_arg *mocarg_init();
void mocarg_destroy(moc_arg *arg);

int compare_servers(const void *s1, const void *s2);
moc_srv *select_srv(moc_t *evt, const char *key, size_t ksize);
ssize_t srecv(int fd, unsigned char *buf, size_t count, int flags);
ssize_t ssend(int fd, const unsigned char *buf, size_t count, int flags);
void close_srv(moc_t *evt, int order, int fd);

#ifdef EVENTLOOP
void process_buf_srv(moc_t *evt, int order, int fd,
                     unsigned char *buf, size_t size, moc_arg *arg);
#endif

__END_DECLS
#endif  /* __INTERNAL_H__ */
