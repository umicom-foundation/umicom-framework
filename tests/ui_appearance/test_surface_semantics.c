/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_surface_semantics.c
 *
 * PURPOSE:
 *   Verify describe semantic surface hierarchy and elevation intent independently of renderer primitives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/surface_semantics.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceSurfaceSemantics item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_surface_semantics_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_surface_semantics_is_valid(&item)) return 2;
    return 0;
}
