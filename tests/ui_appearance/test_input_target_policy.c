/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_input_target_policy.c
 *
 * PURPOSE:
 *   Verify resolve minimum interactive target dimensions by pointer, touch, keyboard or hybrid modality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/input_target_policy.h"
int main(void) {
    UmiAppearanceInputTargetPolicy item;
    if (umi_appearance_input_target_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_input_target_policy_is_valid(&item)) return 2;
    if (umi_appearance_input_target_policy_for_modality(&item,UMI_APPEARANCE_INPUT_TOUCH)!=UMI_STATUS_OK || item.minimum_width_dp!=44.0) return 3;
    return 0;
}
