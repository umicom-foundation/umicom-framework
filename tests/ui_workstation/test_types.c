/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/ui/workstation/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char text[16];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(text, sizeof(text), "layout") != UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_ws_id_valid("studio.code")) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_clamp_unit(1.5) != 1.0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_clamp_i32(50, 0, 40) != 40) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_hash_text("layout") == 0U) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_domain_text(UMI_WS_DOMAIN_TRADER)[0] != 't') return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_surface_kind_text(UMI_WS_SURFACE_TIMELINE)[0] != 't') return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_dock_region_text(UMI_WS_DOCK_LEFT)[0] != 'l') return 8;
    puts("workstation types: ok");
    return 0;
}
