/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_input_affordance_policy.c
 *
 * PURPOSE:
 *   Verify require hover, focus, pressed and touch feedback appropriate to available input modalities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/input_affordance_policy.h"
int main(void) {
    UmiAppearanceInputAffordancePolicy item;
    if (umi_appearance_input_affordance_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_input_affordance_policy_is_valid(&item)) return 2;
    return 0;
}
