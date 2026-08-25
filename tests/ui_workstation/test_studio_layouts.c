#include <stdio.h>
#include "umicom/ui/workstation/studio_layouts.h"
int main(void) {
    UmiWsLayoutTemplate l;
    if (umi_ws_studio_layout_welcome(&l) != UMI_STATUS_OK || l.count != 6U) return 1;
    if (umi_ws_studio_layout_code(&l) != UMI_STATUS_OK || l.count != 5U) return 2;
    if (umi_ws_studio_layout_debug(&l) != UMI_STATUS_OK || l.count != 5U) return 3;
    if (umi_ws_studio_layout_designer(&l) != UMI_STATUS_OK || l.count != 4U) return 4;
    if (umi_ws_studio_layout_diff_merge(&l) != UMI_STATUS_OK || l.count != 3U) return 5;
    if (umi_ws_studio_layout_testing(&l) != UMI_STATUS_OK || l.count != 4U) return 6;
    if (umi_ws_studio_layout_database(&l) != UMI_STATUS_OK || l.count != 4U) return 7;
    if (umi_ws_studio_layout_ai_development(&l) != UMI_STATUS_OK || l.count != 5U) return 8;
    if (umi_ws_studio_layout_ai_teacher(&l) != UMI_STATUS_OK || l.count != 5U) return 9;
    if (umi_ws_studio_layout_focus(&l) != UMI_STATUS_OK || l.count != 1U) return 10;
    puts("studio layouts: ok");
    return 0;
}
