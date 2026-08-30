/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_component_style_binding.c
 *
 * PURPOSE:
 *   Verify bind a semantic component and state map to a Framework style identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/component_style_binding.h"
int main(void) {
    UmiAppearanceComponentStyleBinding item;
    if (umi_appearance_component_style_binding_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_component_style_binding_is_valid(&item)) return 2;
    return 0;
}
