#include "mheads.h"
#include "lheads.h"

HDF *g_cfg = NULL;

#define COL_TRACK " aid, uid, max(ip) AS ip, max(area) AS area "

char* city2area(char city[])
{
    if (!city) return NULL;

    char *p = strstr(city, "省");
    if (!p) p = strstr(city, "市");
    if (!p) return city;

    *p = '\0';

    return city;
}

NEOERR* fill_trackarea(HASH *evth, HASH *dbh)
{
    mdb_conn *conn = (mdb_conn*)hash_lookup(dbh, "dyn");
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "place");
    HDF *node; hdf_init(&node);
    char *ip, *aid, *uid, *city, *area;
    NEOERR *err;

    LPRE_ALLOP(node, conn, evt);

    MDB_QUERY_RAW(conn, "track", COL_TRACK, "intime > current_date AND "
                  " area='' GROUP BY aid, uid", NULL);
    err = mdb_set_rows(node, conn, COL_TRACK, NULL, NULL);
    if (err != STATUS_OK) return nerr_pass(err);

    node = hdf_obj_child(node);
    while (node) {
        ip = hdf_get_value(node, "ip", NULL);
        aid = hdf_get_value(node, "aid", "0");
        uid = hdf_get_value(node, "uid", "0");
        if (ip) {
            hdf_set_value(evt->hdfsnd, "ip", ip);
            MEVENT_TRIGGER_NRET(evt, ip, REQ_CMD_PLACEGET, FLAGS_SYNC);
            city = hdf_get_value(evt->hdfrcv, "0.c", "Mars");
            area = city2area(city);
            MDB_EXEC(conn, NULL, "UPDATE track SET area=$1 WHERE aid=$2 AND uid=$3",
                     "sss", area, aid, uid);
        }

        node = hdf_obj_next(node);
    }

    return STATUS_OK;
}

int main(int argc, char *argv[])
{
    NEOERR *err = STATUS_OK;
    HASH *dbh, *evth;

    mtc_init(TC_ROOT"prestat");

    err = lerr_init();
    DIE_NOK_MTL(err);

    err = mconfig_parse_file(SITE_CONFIG, &g_cfg);
    DIE_NOK_MTL(err);

    err = ldb_init(&dbh);
    DIE_NOK_MTL(err);

    err = levt_init(&evth);
    DIE_NOK_MTL(err);

    err = fill_trackarea(evth, dbh);
    TRACE_NOK(err);

    levt_destroy(evth);
    ldb_destroy(dbh);
    mconfig_cleanup(&g_cfg);
    return 0;
}

