#include <stdio.h>
#include "umicom/ui/workstation/layout_variant.h"
int main(void) {
    UmiWsLayoutEnvironment e = {3440,1440,1U,100};
    if (umi_ws_layout_variant_select(&e) != UMI_WS_LAYOUT_ULTRAWIDE) return 1;
    e.monitor_count = 3U;
    if (umi_ws_layout_variant_select(&e) != UMI_WS_LAYOUT_TRIPLE_MONITOR) return 2;
    if (umi_ws_layout_variant_text(UMI_WS_LAYOUT_LAPTOP)[0] != 'l') return 3;
    puts("layout variant: ok");
    return 0;
}
