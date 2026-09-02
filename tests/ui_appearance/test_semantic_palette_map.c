/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_semantic_palette_map.c
 *
 * PURPOSE:
 *   Verify map a semantic colour role to a Design-System token identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/semantic_palette_map.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceSemanticPaletteMap item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_semantic_palette_map_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_semantic_palette_map_is_valid(&item)) return 2;
    return 0;
}
