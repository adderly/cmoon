#include "mheads.h"
#include "lheads.h"
#include "oblog.h"

HDF *g_cfg = NULL;

static NEOERR* rend_blog_index(HASH *dbh, HASH *tplh, int pageid, int *pgttr)
{
    CSPARSE *cs = NULL;
    HDF *hdf, *dhdf;
    STRING str;
    NEOERR *err = STATUS_OK;
    char fname[_POSIX_PATH_MAX];
    
    if (!dbh || !tplh) return nerr_raise(NERR_ASSERT, "paramter null");
    
    cs = (CSPARSE*)hash_lookup(tplh, "blog_index");
    dhdf = (HDF*)hash_lookup(tplh, "blog_index_hdf");
    if (!cs || !dhdf) return nerr_raise(LERR_MISS_TPL, "blog_index not found");

    err = hdf_init(&hdf);
    if (err != STATUS_OK) return nerr_pass(err);

    hdf_copy(hdf, NULL, dhdf);

    ltpl_prepare_rend(hdf, "layout.html");

    hdf_set_int_value(hdf, PRE_QUERY".pageid", pageid);

    err = blog_index_static_get(hdf, dbh);
    if (err != STATUS_OK) goto done;

    int ntt = hdf_get_int_value(hdf, PRE_OUTPUT".ntt", 0);
    int pgtt = hdf_get_int_value(hdf, PRE_OUTPUT".pgtt", 1);
    if (pgttr) *pgttr = pgtt;
    if (pageid == 0) {
        if (pgtt > 1) {
            err = hdf_set_int_value(hdf, PRE_OUTPUT".pgprev", pgtt-1);
            TRACE_NOK(err);

            if (ntt % BLOG_NUM_PERPAGE == 1) {
                err = rend_blog_index(dbh, tplh, pgtt-1, NULL);
                TRACE_NOK(err);

                if (pgtt > 2) {
                    /* origin 1.html's nex is index.html, change them into 2.html */
                    err = rend_blog_index(dbh, tplh, pgtt-2, NULL);
                    TRACE_NOK(err);
                }
            }
        }
    } else {
        if (pageid > 1 && pgtt > 1)
            hdf_set_int_value(hdf, PRE_OUTPUT".pgprev", pageid-1);
        if (pgtt == pageid+1)
            hdf_set_value(hdf, PRE_OUTPUT".pgnext", "index");
        else if (pgtt > pageid)
            hdf_set_int_value(hdf, PRE_OUTPUT".pgnext", pageid+1);
    }
    
    cs->hdf = hdf;

    string_init(&str);
    err = cs_render(cs, &str, mcs_strcb);
    if (err != STATUS_OK) goto done;

    if (pageid == 0)
        snprintf(fname, sizeof(fname), "%sindex.html", PATH_BLOG);
    else
        snprintf(fname, sizeof(fname), "%s%d.html", PATH_BLOG, pageid);
    
    err = mutil_makesure_dir(fname);
    if (err != STATUS_OK) goto done;
    
    err = mcs_str2file(str, fname);
    if (err != STATUS_OK) goto done;

#ifdef DEBUG_HDF
    hdf_write_file(hdf, TC_ROOT"hdf.blg.index");
#endif

done:
    hdf_destroy(&hdf);
    cs->hdf = NULL;
    string_clear(&str);
    return nerr_pass(err);
}

static NEOERR* rend_blog(HASH *dbh, HASH *tplh, int bid)
{
    CSPARSE *cs = NULL;
    HDF *hdf, *dhdf;
    STRING str;
    NEOERR *err;
    char fname[_POSIX_PATH_MAX];
    
    if (!dbh || !tplh || bid < 0) return nerr_raise(NERR_ASSERT, "paramter null");
    
    cs = (CSPARSE*)hash_lookup(tplh, "blog");
    dhdf = (HDF*)hash_lookup(tplh, "blog_hdf");
    if (!cs || !dhdf) return nerr_raise(LERR_MISS_TPL, "blog_index not found");

    err = hdf_init(&hdf);
    if (err != STATUS_OK) return nerr_pass(err);

    hdf_copy(hdf, NULL, dhdf);

    ltpl_prepare_rend(hdf, "layout.html");
    
    hdf_set_int_value(hdf, PRE_QUERY".bid", bid);

    err = blog_static_get(hdf, dbh);
    if (err != STATUS_OK) goto done;
    
    hdf_set_copy(hdf, PRE_LAYOUT".title", PRE_OUTPUT".blog.title");
    
    cs->hdf = hdf;

    string_init(&str);
    err = cs_render(cs, &str, mcs_strcb);
    if (err != STATUS_OK) goto done;
    
    snprintf(fname, sizeof(fname), "%s%d/%d.html",
             PATH_BLOG, bid%BLOG_SUBDIR_NUM, bid);

    err = mutil_makesure_dir(fname);
    if (err != STATUS_OK) goto done;

    err = mcs_str2file(str, fname);
    if (err != STATUS_OK) goto done;

#ifdef DEBUG_HDF
    hdf_write_file(hdf, TC_ROOT"hdf.blg");
#endif

done:
    hdf_destroy(&hdf);
    cs->hdf = NULL;
    string_clear(&str);
    return nerr_pass(err);
}

void useage(char *prg)
{
    printf("usage: %s -i id -r -b id\n"
           "\t -i process index id.\n"
           "\t -r process index recurse.\n"
           "\t -b process blog id.\n"
           "example:\n"
           "\t %s -b 22 -i 0\n",
           prg, prg);
    exit(1);
}

int main(int argc, char *argv[])
{
    HASH *tplh = NULL, *dbh = NULL;
    NEOERR *err;
    int c, bid = 0, indexid = -1, pgtt;
    bool dorecurse = false;

    mtc_init(TC_ROOT"blg");

    err = mconfig_parse_file(SITE_CONFIG, &g_cfg);
    DIE_NOK_MTL(err);

    while ( (c=getopt(argc, argv, "b:i:r")) != -1 ) {
        switch(c) {
        case 'b':
            bid = atoi(optarg);
            break;
        case 'i':
            indexid = atoi(optarg);
            break;
        case 'r':
            dorecurse = true;
            break;
        default:
            useage(argv[0]);
        }
    }

    err = ldb_init(&dbh);
    DIE_NOK_MTL(err);
    
    err = hash_init(&tplh, hash_str_hash, hash_str_comp);
    DIE_NOK_MTL(err);

    err = ltpl_parse_file(dbh, NULL, PATH_PAGER, "blog.hdf", tplh);
    DIE_NOK_MTL(err);

    if (indexid >= 0) {
        err = rend_blog_index(dbh, tplh, indexid, &pgtt);
        TRACE_NOK(err);

        if (indexid > 0 && pgtt > indexid) pgtt = indexid;
        if (dorecurse) {
            while (pgtt-- > 0) {
                err = rend_blog_index(dbh, tplh, pgtt, NULL);
                TRACE_NOK(err);
            }
        }
    }
    
    if (bid > 0) {
        if (bid > 1)
            rend_blog(dbh, tplh, bid-1);
        rend_blog(dbh, tplh, bid);
        rend_blog(dbh, tplh, bid+1);
    }

    ldb_destroy(dbh);
    ltpl_destroy(tplh);
    mconfig_cleanup(&g_cfg);

    return 0;
}
