#include <stdio.h>
#include "umicom/ui/workstation/focus_mode.h"
int main(void) {
    UmiWsFocusMode m;
    if (umi_ws_focus_mode_enter(&m, "editor", 4U) != UMI_STATUS_OK) return 1;
    if (!umi_ws_focus_mode_should_hide(&m, "terminal")) return 2;
    if (umi_ws_focus_mode_should_hide(&m, "editor")) return 3;
    umi_ws_focus_mode_exit(&m);
    if (m.active) return 4;
    puts("focus mode: ok");
    return 0;
}
