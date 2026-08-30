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
int main(void) {
    UmiAppearanceSurfaceSemantics item;
    if (umi_appearance_surface_semantics_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_surface_semantics_is_valid(&item)) return 2;
    return 0;
}
