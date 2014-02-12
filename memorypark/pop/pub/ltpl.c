#include "mheads.h"
#include "lheads.h"

static int tpl_config(const struct dirent *ent)
{
    if (reg_search(".*.hdf", ent->d_name))
        return 1;
    else
        return 0;
}

void ltpl_prepare_rend(HDF *hdf, char *tpl)
{
    char key[LEN_ST];
    HDF *tmphdf, *ahdf; 
    if (hdf == NULL) return;

    /*
     * set template variable
     */
    hdf_set_copy(hdf, PRE_OUT_TPL".uri", PRE_REQ_URI_RW);
    
    /*
     * merge dataset from g_cfg 
     */
    snprintf(key, sizeof(key), PRE_CFG_DATASET".%s", tpl);
    tmphdf = hdf_get_obj(g_cfg, key);
    if (tmphdf != NULL) hdf_copy(hdf, PRE_CFG_LAYOUT, tmphdf);
    
    /*
     * special actions
     */
    tmphdf = hdf_get_child(hdf, PRE_SPECIAL_ACTION);
    while (tmphdf) {
        snprintf(key, sizeof(key), PRE_LAYOUT".%s", hdf_obj_value(tmphdf));
        ahdf = hdf_get_obj(hdf, key);
        if (ahdf) hdf_copy(hdf, PRE_LAYOUT".actions", ahdf);
        
        tmphdf = hdf_obj_next(tmphdf);
    }
    
    /*
     * set classes
     */
    char *pos = hdf_get_value(hdf, "Layout.tabpart", NULL);
    if (pos) hdf_set_valuef(hdf, "Layout.tabs.%s.class=selected", pos);
    pos = hdf_get_value(hdf, "Layout.actionpart", NULL);
    if (pos) hdf_set_valuef(hdf, "Layout.actions.%s.class=selected", pos);
}

NEOERR* ltpl_parse_file(HASH *dbh, void *lib, char *dir, char *name, HASH *outhash)
{
    char *tp = NULL, *tpl = NULL, *val = NULL;
    HDF *node = NULL, *dhdf = NULL, *child = NULL;
    CSPARSE *cs = NULL;
    STRING str;
    char fname[_POSIX_PATH_MAX], tok[64];
    NEOERR* (*data_handler)(HDF *hdf, HASH *dbh);
    NEOERR *err;
    
    memset(fname, 0x0, sizeof(fname));
    snprintf(fname, sizeof(fname), "%s/%s", dir, name);
    err = hdf_init(&node);
    if (err != STATUS_OK) return nerr_pass(err);

    err = hdf_read_file(node, fname);
    if (err != STATUS_OK) return nerr_pass(err);

    child = hdf_obj_child(node);
    while (child != NULL) {
        mtc_dbg("parse node %s", hdf_obj_name(child));
        string_init(&str);

        val = mutil_obj_attr(child, "merge");
        if (val) {
            snprintf(fname, sizeof(fname), "%s/%s", dir, val);
            err = hdf_init(&dhdf);
            JUMP_NOK(err, wnext);
            err = hdf_read_file(dhdf, fname);
            JUMP_NOK(err, wnext);
            err = hdf_copy(child, NULL, dhdf);
            JUMP_NOK(err, wnext);
        }
        
        err = cs_init(&cs, hdf_get_obj(child, PRE_CFG_DATASET));
        JUMP_NOK(err, wnext);
            
        hdf_set_value(cs->hdf, "hdf.loadpaths.local", dir);

        err = cgi_register_strfuncs(cs);
        JUMP_NOK(err, wnext);
        err = mcs_register_bitop_functions(cs);
        JUMP_NOK(err, wnext);
        err = mcs_register_mkd_functions(cs);
        JUMP_NOK(err, wnext);

        tpl = hdf_get_value(child, PRE_CFG_LAYOUT, "null.html");
        snprintf(fname, sizeof(fname), "%s/%s", PATH_TPL, tpl);
        err = cs_parse_file(cs, fname);
        JUMP_NOK(err, wnext);

        if (outhash != NULL) {
            /*
             * strdup the key, baby, because we'll free the hdf later
             */
            err = hash_insert(outhash, (void*)strdup(hdf_obj_name(child)), (void*)cs);
            JUMP_NOK(err, wnext);
            if (hdf_get_obj(child, PRE_CFG_DATASET)) {
                err = hdf_init(&dhdf);
                JUMP_NOK(err, wnext);
                err = hdf_copy(dhdf, NULL, hdf_get_obj(child, PRE_CFG_DATASET));
                JUMP_NOK(err, wnext);
                snprintf(tok, sizeof(tok), "%s_hdf", hdf_obj_name(child));
                err = hash_insert(outhash, (void*)strdup(tok), (void*)dhdf);
                JUMP_NOK(err, wnext);
            }
        }
            
        if (hdf_get_value(child, PRE_CFG_OUTPUT, NULL) != NULL) {
            ltpl_prepare_rend(hdf_get_obj(child, PRE_CFG_DATASET), tpl);
                
            /*
             * get_data
             */
            val = hdf_get_value(child, PRE_CFG_DATAER, NULL);
            if (val != NULL && lib) {
                data_handler = dlsym(lib, val);
                if( (tp = dlerror()) != NULL) {
                    mtc_err("%s", tp);
                    //continue;
                } else {
                    err = (*data_handler)(hdf_get_obj(child, PRE_CFG_DATASET), dbh);
                    TRACE_NOK(err);
                }
            }
                
            err = cs_render(cs, &str, mcs_strcb);
            JUMP_NOK(err, wnext);
                
            snprintf(fname, sizeof(fname), PATH_DOC"%s",
                     hdf_get_value(child, PRE_CFG_OUTPUT, "null.html"));
            err = mutil_makesure_dir(fname);
            JUMP_NOK(err, wnext);

            err = mcs_str2file(str, fname);
            JUMP_NOK(err, wnext);
#ifdef DEBUG_HDF
            snprintf(fname, sizeof(fname), "%s/hdf.%s",
                     TC_ROOT, hdf_obj_name(child));
            hdf_write_file(child, fname);
#endif
        }

    wnext:
        if (cs != NULL && outhash == NULL)
            cs_destroy(&cs);
        string_clear(&str);
        child = hdf_obj_next(child);
    }
        
    if (node != NULL) hdf_destroy(&node);

    return STATUS_OK;
}

NEOERR* ltpl_parse_dir(char *dir, HASH *outhash)
{
    struct dirent **eps = NULL;
    int n;
    NEOERR *err;

    if (!dir) return nerr_raise(NERR_ASSERT, "can't read null directory");

    HASH *dbh;
    void *lib = dlopen(NULL, RTLD_NOW|RTLD_GLOBAL);
    if (!lib) return nerr_raise(NERR_SYSTEM, "dlopen %s", dlerror());
    
    err = ldb_init(&dbh);
    if (err != STATUS_OK) return nerr_pass(err);
    
    n = scandir(dir, &eps, tpl_config, alphasort);
    for (int i = 0; i < n; i++) {
        mtc_dbg("parse file %s", eps[i]->d_name);
        err = ltpl_parse_file(dbh, lib, dir, eps[i]->d_name, outhash);
        TRACE_NOK(err);
        free(eps[i]);
    }

    ldb_destroy(dbh);
    dlclose(lib);
    
    if (n > 0) free(eps);
    else mtc_warn("not .hdf file found in %s", dir);

    return STATUS_OK;
}

NEOERR* ltpl_init(HASH **tplh)
{
    HASH *ltplh = NULL;
    NEOERR *err;

    *tplh = NULL;
    
    err = hash_init(&ltplh, hash_str_hash, hash_str_comp);
    if (err != STATUS_OK) return nerr_pass(err);

    err = ltpl_parse_dir(PATH_TPL, ltplh);
    if (err != STATUS_OK) return nerr_pass_ctx(err, "pase dir %s", PATH_TPL);
    
    *tplh = ltplh;
    return STATUS_OK;
}

void ltpl_destroy(HASH *tplh)
{
    char *key = NULL;
    
    char *buf = (char*)hash_next(tplh, (void**)&key);

    while (buf != NULL) {
        free(buf);
        buf = hash_next(tplh, (void**)&key);
    }

    hash_destroy(&tplh);
}

NEOERR* ltpl_render(CGI *cgi, HASH *tplh, session_t *ses)
{
    CSPARSE *cs;
    HDF *dhdf;
    STRING str;
    NEOERR *err;

    char *render = NULL, tok[64];

    render = ses->render;
    cs = (CSPARSE*)hash_lookup(tplh, render);
    if (!cs) return nerr_raise(LERR_MISS_TPL, "render %s not found", render);

    snprintf(tok, sizeof(tok), "%s_hdf", render);
    dhdf = (HDF*)hash_lookup(tplh, tok);
    if (dhdf) hdf_copy(cgi->hdf, NULL, dhdf);
    
    ltpl_prepare_rend(cgi->hdf, "layout.html");
    
    if (ses->tm_cache_browser > 0) {
        hdf_set_valuef(cgi->hdf, "cgiout.other.cache=Cache-Control: max-age=%lu",
                       ses->tm_cache_browser);
    }
    cs->hdf = cgi->hdf;

    string_init(&str);
    err = cs_render(cs, &str, mcs_strcb);
    if (err != STATUS_OK) return nerr_pass(err);

    err = cgi_output(cgi, &str);
    if (err != STATUS_OK) return nerr_pass(err);

    cs->hdf = NULL;
    string_clear(&str);

    return STATUS_OK;
}
