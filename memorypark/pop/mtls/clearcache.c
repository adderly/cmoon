#include "mheads.h"
#include "lheads.h"

int main(int argc, char *argv[])
{
    int ret;
    mevent_t *evt = mevent_init_plugin(argv[1]);
    if (!evt) {
        printf("init %s error\n", argv[1]);
        return 1;
    }

    ret = mevent_trigger(evt, NULL, 103, FLAGS_SYNC);
    if (PROCESS_OK(ret))
        hdf_dump(evt->hdfrcv, NULL);
    else
        printf("process failure %d\n", ret);

    mevent_free(evt);
    return 0;
}
