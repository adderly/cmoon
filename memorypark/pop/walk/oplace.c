#include "mheads.h"
#include "lheads.h"
#include "oplace.h"

static iconv_t cv = NULL;
static struct cache *ipc = NULL;
static unsigned char *ips = NULL;
static unsigned int ipbgn, ipend;

static char* gb2utf8(char *s)
{
    if (!s) return NULL;

    if (!cv || (int)cv == -1) cv = iconv_open("UTF-8", "GB2312");
    if ((int)cv == -1) {
        mtc_err("init conv error %s", strerror(errno));
        return NULL;
    }

    unsigned int len = strlen(s), ulen;
    ulen = len*2;
    char *utf8 = calloc(1, ulen);
    char *us = utf8;
    
    if (iconv (cv, &s, &len, &utf8, &ulen) == -1) {
        mtc_err("conv error");
        free(us);
        return NULL;
    }
    
    //iconv_close(cv);
    return us;
}

static int ip_cache_get(char *ip, char **c, char **a)
{
    char *val = NULL;
    size_t vsize = 0;
    
    if (ipc == NULL) ipc = cache_create(1024, 0);
    
    int hit = cache_get(ipc, (unsigned char*)ip, strlen(ip),
                        (unsigned char**)&val, &vsize);
    if (hit != 0) {
        *c = val;
        char *x = strchr(val, ';');
        if (x) {
            *x = '\0';
            *a = x+1;
        } else {
            *a = val + strlen(val) +1;
        }
        return 0;
    }
    
    return -1;
}

static void ip_cache_set(char *ip, char *a, char *c)
{
    if (ipc == NULL) ipc = cache_create(1024, 0);

    char tok[1024] = {0};
    snprintf(tok, sizeof(tok), "%s;%s", a, c);

    cache_set(ipc, (unsigned char*)ip, strlen(ip),
              (unsigned char*)tok, strlen(tok)+2, 0);
}

static unsigned int b2int(unsigned char *p, int count)
{
    int i;
    unsigned int ret;

    if(count < 1 || count > 4) 
        return 0;
    
    ret = p[0];
    for (i = 0; i < count; i++)
        ret |= ((unsigned int)p[i])<<(8*i);
    
    return ret;
}

static void load_ipfile()
{
    if (ips == NULL) {
        NEOERR *err = ne_load_file(PATH_MTLS"QQWry.Dat", (char**)&ips);
        RETURN_NOK(err);

        ipbgn = b2int(ips, 4);
        ipend = b2int(ips+4, 4);

        if (ipbgn == -1 || ipend == -1) {
            mtc_err("file format error");
            if (ips) free(ips);
            ips = NULL;
        }
    }
}

static unsigned char* readarea(int offset)
{
    if (!ips) return NULL;
    
    unsigned char *p = ips + offset;
    unsigned char mode = *p;
    
    if (mode == REDIRECT_MODE_1 || mode == REDIRECT_MODE_2) {
        offset = b2int(p+1, 3);
        if (offset == 0) {
            return NULL;
        }
    }
    return ips + offset;
}

static void ip_place(int offset, char **c, char **a)
{
    if (!ips) return;
    
    unsigned char *p = ips + offset + 4;
    unsigned char mode = *p;

    if (mode == REDIRECT_MODE_1) {
        offset = b2int(p+1, 3);
        p = ips + offset;
        mode = *p;
        if (mode == REDIRECT_MODE_2) {
            *c = (char*)(ips + b2int(p+1, 3));
            *a = (char*)readarea(offset+4);
        } else {
            *c = (char*)(ips + offset);
            *a = (char*)readarea(offset + strlen(*c) + 1);
        }
    } else if (mode == REDIRECT_MODE_2) {
        offset = b2int(p+1, 3);
        *c = (char*)(ips + offset);
        *a = (char*)readarea(offset+4+4);
    } else {
        *c = (char*)(ips + offset + 4);
        *a = (char*)(readarea(offset + 4 + strlen(*c) + 1));
    }
    *c = gb2utf8(*c);
    *a = gb2utf8(*a);
}

static int ip_offset(unsigned int ip)
{
    unsigned int ipb, ipe;
    unsigned int M, L, R, record_count;

    record_count = (ipend - ipbgn)/7+1;
    /* search for right range */
    L = 0;
    R = record_count - 1;
    while (L < R-1) {
        M = (L + R) / 2;
        ipb = b2int(ips + ipbgn + M*7, 4);
        ipe = b2int(ips + b2int(ips + ipbgn + M*7 + 4, 3), 4);

        if (ip == ipb) {
            L = M;
            break;
        }
        if (ip > ipb)
            L = M;
        else
            R = M;
    }

    ipb = b2int(ips + ipbgn + L*7, 4);
    ipe = b2int(ips + b2int(ips + ipbgn + L*7 + 4, 3), 4);

#if 0
    /* version infomation, the last item */
    if((ip & 0xffffff00) == 0xffffff00)
        set_ip_range(R, &f);
#endif
    
    if (ipb <= ip && ip <= ipe)
        return b2int(ips + ipbgn + L*7 + 4, 3);
    else
        return -1;
}

NEOERR* ip2addr_data_get(char *ip, char **c, char **a)
{
    *c = *a = NULL;
    
    if (!ip) return nerr_raise(NERR_ASSERT, "ip null");

    char *p, *s;
    unsigned int dip = 0;

    if (ip_cache_get(ip, c, a) == 0) {
        mtc_dbg("get %s from cache: %s %s", ip, *c, *a);
        return STATUS_OK;
    }
    
    s = strdup(ip);
    p = strtok(s, ".");
    while (p) {
        dip <<= 8;
        dip |= (unsigned int)atoi(p);
        p = strtok(NULL, ".");
    }
    free(s);

    load_ipfile();
    if (ips) {
        int offset = ip_offset(dip);
        if (offset != -1) {
            ip_place(offset, c, a);
            ip_cache_set(ip, *c, *a);
            free(*c);
            free(*a);
            ip_cache_get(ip, c, a);
            mtc_dbg("get %s from file: %s %s", ip, *c, *a);
            return STATUS_OK;
        }
    }

    return nerr_raise(NERR_SYSTEM, PATH_MTLS"QQWry.Dat nexist or format error");
}

NEOERR* place_data_get_local(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    if (!cgi || !cgi->hdf) return nerr_raise(NERR_ASSERT, "paramter null");

    char *ip, *c, *a;
    NEOERR *err;

    HDF_GET_STR(cgi->hdf, PRE_QUERY".ip", ip);

    c = a = NULL;
    err = ip2addr_data_get(ip, &c, &a);
    if (err != STATUS_OK) return nerr_pass(err);
    hdf_set_value(cgi->hdf, PRE_OUTPUT".a", a);
    hdf_set_value(cgi->hdf, PRE_OUTPUT".c", c);
    
    return STATUS_OK;
}

NEOERR* place_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "place");
    char *ip;
    
    /*
     * input check
     */
    LPRE_EVTOP(cgi->hdf, evt);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".ip", ip);

    /*
     * prepare data 
     */
    hdf_set_value(evt->hdfsnd, "ip", ip);
    
    /*
     * trigger
     */
    MEVENT_TRIGGER(evt, ip, REQ_CMD_PLACEGET, FLAGS_SYNC);

    /*
     * set output
     */
    if (evt->hdfrcv) {
        hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    }

    return STATUS_OK;
}
