#include <stdio.h>
#include "umicom/ui/workstation/os_layouts.h"
int main(void) {
    UmiWsLayoutTemplate l;
    if (umi_ws_os_layout_desktop(&l) != UMI_STATUS_OK || l.count != 3U) return 1;
    if (umi_ws_os_layout_systems(&l) != UMI_STATUS_OK || l.count != 4U) return 2;
    if (umi_ws_os_layout_monitor(&l) != UMI_STATUS_OK || l.count != 4U) return 3;
    puts("os layouts: ok");
    return 0;
}
