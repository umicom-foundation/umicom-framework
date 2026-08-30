/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_motion_policy.c
 *
 * PURPOSE:
 *   Verify define semantic motion allowances and maximum transition durations for production UI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/motion_policy.h"
int main(void) {
    UmiAppearanceMotionPolicy item;
    if (umi_appearance_motion_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_motion_policy_is_valid(&item)) return 2;
    return 0;
}
