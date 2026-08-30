/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_surface_style_projection.c
 *
 * PURPOSE:
 *   Verify resolve semantic surface roles to token identities consumed by frontend renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/surface_style_projection.h"
int main(void) {
    UmiAppearanceSurfaceStyleProjection item;
    if (umi_appearance_surface_style_projection_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_surface_style_projection_is_valid(&item)) return 2;
    return 0;
}
