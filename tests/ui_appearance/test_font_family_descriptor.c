/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_family_descriptor.c
 *
 * PURPOSE:
 *   Verify describe one semantic font-family candidate and its broad typographic classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_family_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceFontFamilyDescriptor item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_font_family_descriptor_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_font_family_descriptor_is_valid(&item)) return 2;
    return 0;
}
