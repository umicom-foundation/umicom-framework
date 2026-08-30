/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_application_brand_binding.c
 *
 * PURPOSE:
 *   Verify bind a thin application identity to Framework-owned brand and theme-pack identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/application_brand_binding.h"
int main(void) {
    UmiAppearanceApplicationBrandBinding item;
    if (umi_appearance_application_brand_binding_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_application_brand_binding_is_valid(&item)) return 2;
    return 0;
}
