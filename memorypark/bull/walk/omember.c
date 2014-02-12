#include "mheads.h"
#include "lheads.h"
#include "omember.h"

static NEOERR* member_after_login(CGI *cgi, HASH *dbh, char *mname)
{
    char tm[LEN_TM_GMT], *p, mmsn[LEN_CK];
    mdb_conn *db = hash_lookup(dbh, "main");
    NEOERR *err;

    memset(mmsn, 0x0, sizeof(mmsn));
    mstr_rand_string(mmsn, sizeof(mmsn));
    mutil_getdatetime_gmt(tm, sizeof(tm), "%A, %d-%b-%Y %T GMT", ONE_WEEK);

    /*
     * set cookie 
     */
    neos_url_escape(mname, &p, NULL);

    cgi_cookie_set(cgi, "mname",     mname, NULL, SITE_DOMAIN, tm, 1, 0);
    cgi_cookie_set(cgi, "mname_esc", p,     NULL, SITE_DOMAIN, tm, 1, 0);
    cgi_cookie_set(cgi, "mmsn",      mmsn,  NULL, SITE_DOMAIN, tm, 1, 0);

    free(p);

    hdf_set_value(cgi->hdf, PRE_OUTPUT".mname", mname);
    hdf_set_value(cgi->hdf, PRE_OUTPUT".mmsn", mmsn);

    MDB_EXEC(db, NULL, "UPDATE member SET mmsn=$1 WHERE mname=$2",
             "ss", mmsn, mname);

    return STATUS_OK;
}

NEOERR* member_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname;
    NEOERR *err;

    return STATUS_OK;
}

NEOERR* member_logout_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname;
    NEOERR *err;
    
    MCS_NOT_NULLB(cgi->hdf, db);

    MEMBER_CHECK_LOGIN();

    MDB_EXEC(db, NULL, "UPDATE member SET mmsn='0' WHERE mname=$1",
             "s", mname);

    return STATUS_OK;
}

NEOERR* member_check_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname, *mmsn;
    NEOERR *err;
    
    MCS_NOT_NULLB(cgi->hdf, db);

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".mname", mname);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".mmsn", mmsn);
    if (!mname || !mmsn) {
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".mname", mname);
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".mmsn", mmsn);
    }
    hdf_set_value(cgi->hdf, PRE_RESERVE".mname", mname);

    MDB_QUERY_RAW(db, "member", _COL_MEMBER, "mname=$1", "s", mname);
    err = mdb_set_row(cgi->hdf, db, _COL_MEMBER, PRE_OUTPUT".member", MDB_FLAG_Z);
    if (err != STATUS_OK) return nerr_pass(err);

    char *mmsndb = hdf_get_value(cgi->hdf, PRE_OUTPUT".member.mmsn", NULL);
    if (!mmsndb || strcmp(mmsndb, mmsn)) {
        hdf_remove_tree(cgi->hdf, PRE_OUTPUT".member");
        return nerr_raise(LERR_NOTLOGIN, "msn don't match");
    }
    
    return STATUS_OK;
}

NEOERR* member_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "main");
    char *mname, *msn;
    NEOERR *err;
    
    MCS_NOT_NULLB(cgi->hdf, db);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".mname", mname); 
    HDF_GET_STR(cgi->hdf, PRE_QUERY".msn", msn);

    MDB_QUERY_RAW(db, "member", _COL_MEMBER, "mname=$1", "s", mname);
    err = mdb_set_row(cgi->hdf, db, _COL_MEMBER, PRE_OUTPUT".member", MDB_FLAG_Z);
    if (err != STATUS_OK) return nerr_pass(err);

    char *msndb = hdf_get_value(cgi->hdf, PRE_OUTPUT".member.msn", NULL);
    if (!msndb || strcmp(msndb, msn)) {
        hdf_remove_tree(cgi->hdf, PRE_OUTPUT".member");
        return nerr_raise(LERR_LOGINPSW, "msn incorrct");
    }

    return nerr_pass(member_after_login(cgi, dbh, mname));
}
