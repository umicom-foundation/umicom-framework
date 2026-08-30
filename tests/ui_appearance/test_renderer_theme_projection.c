/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_renderer_theme_projection.c
 *
 * PURPOSE:
 *   Verify record one renderer-specific projection of a semantic style packet without transferring state ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_theme_projection.h"
int main(void) {
    UmiAppearanceRendererThemeProjection item;
    if (umi_appearance_renderer_theme_projection_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_renderer_theme_projection_is_valid(&item)) return 2;
    return 0;
}
