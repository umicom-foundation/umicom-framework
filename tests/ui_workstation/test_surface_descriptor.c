/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_surface_descriptor.c
 *
 * PURPOSE:
 *   Implement the test surface descriptor behavior for
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
#include "umicom/ui/workstation/surface_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsSurfaceDescriptor d;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_descriptor_init(&d, "studio.editor", "Editor", UMI_WS_DOMAIN_STUDIO, UMI_WS_SURFACE_EDITOR) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_descriptor_validate(&d) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!d.movable || d.minimum_width != 160) return 3;
    puts("surface descriptor: ok");
    return 0;
}
