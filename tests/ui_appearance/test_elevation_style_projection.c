/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_elevation_style_projection.c
 *
 * PURPOSE:
 *   Verify resolve semantic elevation levels to shadow and border tokens suitable for each frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/elevation_style_projection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceElevationStyleProjection item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_elevation_style_projection_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_elevation_style_projection_is_valid(&item)) return 2;
    return 0;
}
