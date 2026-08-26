/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_keyboard_access_policy.c
 *
 * PURPOSE:
 *   Verify define keyboard operability and transient-surface escape requirements for all applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/keyboard_access_policy.h"
int main(void) {
    UmiAppearanceKeyboardAccessPolicy item;
    if (umi_appearance_keyboard_access_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_keyboard_access_policy_is_valid(&item)) return 2;
    return 0;
}
