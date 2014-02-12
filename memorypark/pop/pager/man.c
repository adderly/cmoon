#include "mheads.h"
#include "lheads.h"

HDF *g_cfg = NULL;

static int html_config(const struct dirent *ent)
{
    if (reg_search(".*.html", ent->d_name))
        return 1;
    else
        return 0;
}

int main(int argc, char *argv[])
{
    HASH *tplh = NULL;
    CSPARSE *cs = NULL;
    HDF *hdf, *dhdf;
    STRING str;
    char fname[_POSIX_PATH_MAX];
    NEOERR *err;

    mtc_init(TC_ROOT"man");

    err = mconfig_parse_file(SITE_CONFIG, &g_cfg);
    DIE_NOK_MTL(err);

    err = hash_init(&tplh, hash_str_hash, hash_str_comp);
    DIE_NOK_MTL(err);

    err = ltpl_parse_file(NULL, NULL, PATH_PAGER, "manual.hdf", tplh);
    DIE_NOK_MTL(err);

    cs = (CSPARSE*)hash_lookup(tplh, "manual");
    dhdf = (HDF*)hash_lookup(tplh, "manual_hdf");
    if (!cs || !dhdf) {
        mtc_err("manual config not found");
        return -1;
    }

    struct dirent **eps = NULL;
    int n;

    n = scandir(PATH_PAGER"manual/", &eps, html_config, alphasort);
    for (int i = 0; i < n; i++) {
        mtc_dbg("parse file %s", eps[i]->d_name);
        err = hdf_init(&hdf);
        JUMP_NOK(err, next);

        hdf_copy(hdf, NULL, dhdf);

        /*
         * this require template use linclude not include,
         * to include file on rend time, not parse time
         */
        snprintf(fname, sizeof(fname), "manual/%s", eps[i]->d_name);
        hdf_set_value(hdf, "Include.content", fname);
        ltpl_prepare_rend(hdf, "layout.html");

        cs->hdf = hdf;
        string_init(&str);
        err = cs_render(cs, &str, mcs_strcb);
        JUMP_NOK(err, next);

        snprintf(fname, sizeof(fname), PATH_DOC"manual/%s", eps[i]->d_name);

        err = mutil_makesure_dir(fname);
        JUMP_NOK(err, next);

        err = mcs_str2file(str, fname);
        JUMP_NOK(err, next);

#ifdef DEBUG_HDF
        hdf_write_file(hdf, TC_ROOT"hdf.man");
#endif
    next:
        hdf_destroy(&hdf);
        cs->hdf = NULL;
        string_clear(&str);
        
        free(eps[i]);
    }
    
    ltpl_destroy(tplh);
    mconfig_cleanup(&g_cfg);
    
    return 0;
}
