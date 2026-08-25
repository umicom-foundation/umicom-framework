#include <stdio.h>
#include "umicom/ui/workstation/auto_hide.h"
int main(void) {
    UmiWsAutoHideState s;
    if (umi_ws_auto_hide_init(&s, "outline", UMI_WS_DOCK_LEFT) != UMI_STATUS_OK) return 1;
    if (umi_ws_auto_hide_set_revealed(&s, true) != UMI_STATUS_OK || !s.revealed) return 2;
    if (umi_ws_auto_hide_edge_valid(UMI_WS_DOCK_CENTRE)) return 3;
    puts("auto hide: ok");
    return 0;
}
