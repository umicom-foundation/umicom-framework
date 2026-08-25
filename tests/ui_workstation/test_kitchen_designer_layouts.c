#include <stdio.h>
#include "umicom/ui/workstation/kitchen_designer_layouts.h"
int main(void) {
    UmiWsLayoutTemplate l;
    if (umi_ws_kitchen_layout_design(&l) != UMI_STATUS_OK || l.count != 4U) return 1;
    if (umi_ws_kitchen_layout_review(&l) != UMI_STATUS_OK || l.count != 4U) return 2;
    if (umi_ws_kitchen_layout_catalogue(&l) != UMI_STATUS_OK || l.count != 3U) return 3;
    puts("kitchen layouts: ok");
    return 0;
}
