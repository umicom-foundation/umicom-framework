#include <stdio.h>
#include "umicom/ui/workstation/layout_template.h"
int main(void) {
    UmiWsLayoutTemplate l;
    if (umi_ws_layout_template_init(&l, "studio.code", "Code", UMI_WS_DOMAIN_STUDIO) != UMI_STATUS_OK) return 1;
    if (umi_ws_layout_template_add(&l, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 1, 2.0) != UMI_STATUS_OK) return 2;
    if (umi_ws_layout_template_find(&l, "studio.editor") == NULL) return 3;
    if (umi_ws_layout_template_fingerprint(&l) == 0U) return 4;
    puts("layout template: ok");
    return 0;
}
