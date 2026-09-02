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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceMotionPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_motion_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_motion_policy_is_valid(&item)) return 2;
    return 0;
}
