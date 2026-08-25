#include <stdio.h>
#include "umicom/ui/workstation/perspective_template.h"
int main(void) {
    UmiWsPerspectiveTemplate p;
    if (umi_ws_perspective_template_init(&p, "studio.debug", "Debug", UMI_WS_DOMAIN_STUDIO) != UMI_STATUS_OK) return 1;
    if (umi_ws_perspective_template_set_default_layout(&p, "studio.debug.desktop") != UMI_STATUS_OK) return 2;
    if (umi_ws_perspective_template_add_surface(&p, "debug.variables") != UMI_STATUS_OK) return 3;
    if (p.preferred_surface_count != 1U) return 4;
    puts("perspective template: ok");
    return 0;
}
