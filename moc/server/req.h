#ifndef __REQ_H__
#define __REQ_H__

#define REQTYPE_TCP 2

struct req_info {
    /* network information */
    int fd;
    int type;

    struct sockaddr *clisa;
    socklen_t clilen;

    /* operation information */
    uint32_t id;
    uint16_t cmd;
    uint16_t flags;
    const unsigned char *payload;
    size_t psize;

    /* operations */
    /* reply_err is depracated */
    void (*reply_mini)(const struct req_info *req, uint32_t reply);
    void (*reply_err)(const struct req_info *req, uint32_t reply);
    void (*reply_long)(const struct req_info *req, uint32_t reply,
            unsigned char *val, size_t vsize);
};

#endif  /* __REQ_H__ */
