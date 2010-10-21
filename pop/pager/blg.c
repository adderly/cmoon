#include "mheads.h"
#include "lheads.h"
#include "oblog.h"

HDF *g_cfg = NULL;

static int rend_blog_index(HASH *dbh, HASH *tplh, int pageid)
{
	CSPARSE *cs = NULL;
	HDF *hdf, *dhdf;
	STRING str;
	NEOERR *err;
	char fname[_POSIX_PATH_MAX];
	
	if (!dbh || !tplh) return 0;
	
	cs = (CSPARSE*)hash_lookup(tplh, "blog_index");
	dhdf = (HDF*)hash_lookup(tplh, "blog_index_hdf");
	if (!cs || !dhdf) return 0;

	hdf_init(&hdf);
	hdf_copy(hdf, NULL, dhdf);
	ltpl_prepare_rend(hdf, "layout.html");

	hdf_set_int_value(hdf, PRE_QUERY".pageid", pageid);
	if (blog_index_static_get(hdf, dbh) != RET_RBTOP_OK) {
		hdf_destroy(&hdf);
		return 0;
	}

	int ntt = hdf_get_int_value(hdf, PRE_OUTPUT".ntt", 0);
	int pgtt = hdf_get_int_value(hdf, PRE_OUTPUT".pgtt", 1);
	if (pageid == 0) {
		if (pgtt > 1) {
			hdf_set_int_value(hdf, PRE_OUTPUT".pgprev", pgtt-1);
			if (ntt % BLOG_NUM_PERPAGE == 1)
				rend_blog_index(dbh, tplh, pgtt-1);
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
	RETURN_V_NOK(err, 0);

	if (pageid == 0)
		snprintf(fname, sizeof(fname), "%sindex.html", PATH_BLOG);
	else
		snprintf(fname, sizeof(fname), "%s%d.html", PATH_BLOG, pageid);
	mutil_makesure_dir(fname);
	if (!mcs_str2file(str, fname)) {
		mtc_err("write file %s failure", fname);
	}

#ifdef DEBUG_HDF
	hdf_write_file(hdf, TC_ROOT"hdf.blg.index");
#endif

	hdf_destroy(&hdf);

	cs->hdf = NULL;
	string_clear(&str);

	return pgtt;
}

static void rend_blog(HASH *dbh, HASH *tplh, int bid)
{
	CSPARSE *cs = NULL;
	HDF *hdf, *dhdf;
	STRING str;
	NEOERR *err;
	char fname[_POSIX_PATH_MAX];
	
	if (!dbh || !tplh || bid < 0) return;
	
	cs = (CSPARSE*)hash_lookup(tplh, "blog");
	dhdf = (HDF*)hash_lookup(tplh, "blog_hdf");
	if (!cs || !dhdf) return;

	hdf_init(&hdf);
	hdf_copy(hdf, NULL, dhdf);
	ltpl_prepare_rend(hdf, "layout.html");
	
	hdf_set_int_value(hdf, PRE_QUERY".bid", bid);
	if (blog_static_get(hdf, dbh) != RET_RBTOP_OK) {
		hdf_destroy(&hdf);
		return;
	}
	
	hdf_set_copy(hdf, PRE_LAYOUT".title", PRE_OUTPUT".blog.title");
	
	cs->hdf = hdf;

	string_init(&str);
	err = cs_render(cs, &str, mcs_strcb);
	RETURN_NOK(err);
	
	snprintf(fname, sizeof(fname), "%s%d/%d.html",
			 PATH_BLOG, bid%BLOG_SUBDIR_NUM, bid);
	mutil_makesure_dir(fname);
	if (!mcs_str2file(str, fname)) {
		mtc_err("write file %s failure", fname);
	}

#ifdef DEBUG_HDF
	hdf_write_file(hdf, TC_ROOT"hdf.blg");
#endif
	hdf_destroy(&hdf);

	cs->hdf = NULL;
	string_clear(&str);
}

void useage(char *prg)
{
	printf("usage: %s -i -b id\n"
		   "\t -i process index.\n"
		   "\t -b process blog id.\n"
		   "example:\n"
		   "\t %s -b 22\n",
		   prg, prg);
	exit(1);
}

int main(int argc, char *argv[])
{
	HASH *tplh = NULL, *dbh = NULL;
	NEOERR *err;
	int c, bid = 0;
	bool doindex = false, dorecurse = false;

	mconfig_parse_file(SITE_CONFIG, &g_cfg);
	mtc_init(TC_ROOT"blg");

	while ( (c=getopt(argc, argv, "b:ir")) != -1 ) {
		switch(c) {
		case 'b':
			bid = atoi(optarg);
			break;
		case 'i':
			doindex = true;
			break;
		case 'r':
			dorecurse = true;
			break;
		default:
			useage(argv[0]);
		}
	}

	if (ldb_init(&dbh) != RET_RBTOP_OK) {
		mtc_err("init db error");
		return RET_RBTOP_DBE;
	}
	
	err = hash_init(&tplh, hash_str_hash, hash_str_comp);
	RETURN_V_NOK(err, RET_RBTOP_INITE);
	if (ltpl_parse_file(dbh, NULL, PATH_PAGER, "blog.hdf", tplh) != RET_RBTOP_OK) {
		mtc_err("parse blog.hdf failure");
		return RET_RBTOP_INITE;
	}

	if (doindex) {
		int pgtt = rend_blog_index(dbh, tplh, 0);
		if (dorecurse) {
			while (pgtt-- > 0) {
				rend_blog_index(dbh, tplh, pgtt);
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
