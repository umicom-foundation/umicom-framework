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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceInputTargetPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_input_target_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_input_target_policy_is_valid(&item)) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_input_target_policy_for_modality(&item,UMI_APPEARANCE_INPUT_TOUCH)!=UMI_STATUS_OK || item.minimum_width_dp!=44.0) return 3;
    return 0;
}
