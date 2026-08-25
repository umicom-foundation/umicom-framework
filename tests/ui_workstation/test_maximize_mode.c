#include <stdio.h>
#include "umicom/ui/workstation/maximize_mode.h"
int main(void) {
    UmiWsMaximizeMode m;
    if (umi_ws_maximize_mode_activate(&m, "chart") != UMI_STATUS_OK) return 1;
    if (!umi_ws_maximize_mode_matches(&m, "chart")) return 2;
    umi_ws_maximize_mode_restore(&m);
    if (m.active) return 3;
    puts("maximize mode: ok");
    return 0;
}
