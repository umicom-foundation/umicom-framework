#include <stdio.h>
#include "umicom/ui/workstation/status_strip.h"
int main(void) {
    UmiWsStatusStrip s;
    umi_ws_status_strip_init(&s);
    if (umi_ws_status_strip_set(&s, "mode", "PAPER MODE", UMI_UI_SEVERITY_WARNING, false) != UMI_STATUS_OK) return 1;
    if (umi_ws_status_strip_find(&s, "mode") == NULL) return 2;
    puts("status strip: ok");
    return 0;
}
