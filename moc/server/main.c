#include "main.h"
#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char *argv[])
{
    NEOERR *err;

    static struct setting {
        char *conffname;
        int foreground;
    } myset = {
        .conffname = "./config.hdf",
        .foreground = 0,
    };

    int c;
    while ((c = getopt(argc, argv, "c:f")) != -1) {
        switch(c) {
        case 'c':
            myset.conffname = strdup(optarg);
            break;
        case 'f':
            myset.foreground = 1;
            break;
        }
    }

    err = mcfg_parse_file(myset.conffname, &g_cfg);
    DIE_NOK_MTL(err);
    
    mtc_init(hdf_get_value(g_cfg, PRE_CONFIG".logfile", "/tmp/moc"));
    err = nerr_init();
    RETURN_V_NOK(err, 1);
    err = merr_init((MeventLog)mtc_msg);
    RETURN_V_NOK(err, 1);

    if (!myset.foreground) {
        pid_t pid = fork();
        if (pid > 0) return 0;
        else if (pid < 0) {
            perror("Error in fork");
            return 1;
        }
        close(0);
        setsid();
    }

    mtc_foo("starting moc");

    moc = moc_start();

    net_go();

    moc_stop(moc);

    mcfg_cleanup(&g_cfg);

    return 0;
}
