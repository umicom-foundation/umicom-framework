/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_focus_policy.c
 *
 * PURPOSE:
 *   Verify define visible keyboard-focus treatment requirements across all renderer adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_policy.h"
int main(void) {
    UmiAppearanceFocusPolicy item;
    if (umi_appearance_focus_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_focus_policy_is_valid(&item)) return 2;
    return 0;
}
