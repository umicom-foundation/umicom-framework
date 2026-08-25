#include <stdio.h>
#include "umicom/ui/workstation/dock_target.h"
int main(void) {
    UmiWsDockTarget t;
    UmiUiRect r = {0,0,100,100};
    UmiUiPoint p = {50,50};
    if (umi_ws_dock_target_init(&t, "centre", UMI_WS_DOCK_CENTRE, r) != UMI_STATUS_OK) return 1;
    if (!umi_ws_dock_target_contains(&t, p)) return 2;
    if (umi_ws_dock_target_score(&t, p) <= 0) return 3;
    puts("dock target: ok");
    return 0;
}
