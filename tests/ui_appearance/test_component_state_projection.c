/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_component_state_projection.c
 *
 * PURPOSE:
 *   Verify map semantic component state to resolved style and accessibility state identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/component_state_projection.h"
int main(void) {
    UmiAppearanceComponentStateProjection item;
    if (umi_appearance_component_state_projection_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_component_state_projection_is_valid(&item)) return 2;
    return 0;
}
