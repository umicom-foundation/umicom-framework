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
int main(void) {
    UmiWsSurfaceDescriptor d;
    if (umi_ws_surface_descriptor_init(&d, "studio.editor", "Editor", UMI_WS_DOMAIN_STUDIO, UMI_WS_SURFACE_EDITOR) != UMI_STATUS_OK) return 1;
    if (umi_ws_surface_descriptor_validate(&d) != UMI_STATUS_OK) return 2;
    if (!d.movable || d.minimum_width != 160) return 3;
    puts("surface descriptor: ok");
    return 0;
}
