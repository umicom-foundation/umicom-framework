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
int main(void) {
    UmiAppearanceFontFamilyDescriptor item;
    if (umi_appearance_font_family_descriptor_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_font_family_descriptor_is_valid(&item)) return 2;
    return 0;
}
