#include <stdio.h>
#include "umicom/ui/workstation/resize_policy.h"
int main(void) {
    UmiWsResizePolicy p;
    UmiUiSize min = {100,80}, pref = {300,200}, max = {800,600}, req = {50,900};
    UmiUiSize out;
    if (umi_ws_resize_policy_init(&p, min, pref, max) != UMI_STATUS_OK) return 1;
    out = umi_ws_resize_policy_apply(&p, req);
    if (out.width != 100 || out.height != 600) return 2;
    puts("resize policy: ok");
    return 0;
}
