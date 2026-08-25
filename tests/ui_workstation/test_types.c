#include <stdio.h>
#include "umicom/ui/workstation/types.h"

int main(void) {
    char text[16];
    if (umi_ws_copy_text(text, sizeof(text), "layout") != UMI_STATUS_OK) return 1;
    if (!umi_ws_id_valid("studio.code")) return 2;
    if (umi_ws_clamp_unit(1.5) != 1.0) return 3;
    if (umi_ws_clamp_i32(50, 0, 40) != 40) return 4;
    if (umi_ws_hash_text("layout") == 0U) return 5;
    if (umi_ws_domain_text(UMI_WS_DOMAIN_TRADER)[0] != 't') return 6;
    if (umi_ws_surface_kind_text(UMI_WS_SURFACE_TIMELINE)[0] != 't') return 7;
    if (umi_ws_dock_region_text(UMI_WS_DOCK_LEFT)[0] != 'l') return 8;
    puts("workstation types: ok");
    return 0;
}
