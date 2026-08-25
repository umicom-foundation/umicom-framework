#include <stdio.h>
#include "umicom/ui/workstation/layout_history.h"
int main(void) {
    UmiWsLayoutHistory h;
    UmiWsLayoutTemplate l;
    UmiWsLayoutSnapshot a,b;
    umi_ws_layout_history_init(&h);
    if (umi_ws_layout_template_init(&l, "layout", "Layout", UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK) return 1;
    if (umi_ws_layout_snapshot_capture(&a, "a", &l, 1U, 1, false) != UMI_STATUS_OK) return 2;
    if (umi_ws_layout_snapshot_capture(&b, "b", &l, 2U, 2, false) != UMI_STATUS_OK) return 3;
    if (umi_ws_layout_history_push(&h, &a) != UMI_STATUS_OK || umi_ws_layout_history_push(&h, &b) != UMI_STATUS_OK) return 4;
    if (umi_ws_layout_history_back(&h)->sequence != 1U) return 5;
    if (umi_ws_layout_history_forward(&h)->sequence != 2U) return 6;
    puts("layout history: ok");
    return 0;
}
