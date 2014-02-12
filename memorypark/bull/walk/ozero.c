#include "mheads.h"
#include "lheads.h"
#include "ozero.h"

NEOERR* zero_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "aic");

    MCS_NOT_NULLB(cgi->hdf, evt);

    return STATUS_OK;
}

NEOERR* zero_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "aic");

    MCS_NOT_NULLB(cgi->hdf, evt);

    return STATUS_OK;
}

NEOERR* zero_image_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    char fname[LEN_MD5];
    int ftype;
	NEOERR *err;
    
    err = mimg_accept_and_zoomout(cgi, "upfile", ROOT_IMG, fname, &ftype, 620, 0);
	if (err != STATUS_OK) return nerr_pass(err);
    
    char tok[3] = {0}; strncpy(tok, fname, 2);

    hdf_set_valuef(cgi->hdf, PRE_OUTPUT".imageurl=%s%s/%s.%s",
                   URL_IMG, tok, fname, mimg_type_int2str(ftype));
    hdf_set_valuef(cgi->hdf, PRE_OUTPUT".imageurl_zoom=%s620x0/%s/%s.%s",
                   URL_IMG, tok, fname, mimg_type_int2str(ftype));
    hdf_set_valuef(cgi->hdf, PRE_OUTPUT".imagename=%s.%s",
                   fname, mimg_type_int2str(ftype));

    return STATUS_OK;
}

NEOERR* zero_mp3_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    int mid, stage;
    char *title;
    NEOERR *err;

    HDF_GET_INT(cgi->hdf, PRE_QUERY".mid", mid);
    HDF_GET_INT(cgi->hdf, PRE_QUERY".stage", stage);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".title", title);

    MDB_QUERY_RAW(db, "mp3", "mid", "mid=%d", NULL, mid);
    if (mdb_get_rows(db) > 0) return nerr_raise(NERR_ASSERT, "%d already in", mid);

    MDB_EXEC(db, NULL, "INSERT INTO mp3 (stage, mid, title) VALUES (%d, %d, '%s')",
             NULL, stage, mid, title);

    return STATUS_OK;
}
