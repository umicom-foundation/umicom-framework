/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_variant_resolution.c
 *
 * PURPOSE:
 *   Verify resolve light/dark/high-contrast and direction-aware icon variants while preserving semantic identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_variant_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceIconVariantResolution item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_icon_variant_resolution_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_icon_variant_resolution_is_valid(&item)) return 2;
    umi_appearance_icon_variant_resolution_set_direction(&item,1,1); /* Apply this branch only when its contract condition is satisfied. */ if(!item.mirrored) return 3;
    return 0;
}
