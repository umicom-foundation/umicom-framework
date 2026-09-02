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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceRendererThemeProjection item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_renderer_theme_projection_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_renderer_theme_projection_is_valid(&item)) return 2;
    return 0;
}
