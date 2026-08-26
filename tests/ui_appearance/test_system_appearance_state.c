/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_system_appearance_state.c
 *
 * PURPOSE:
 *   Verify represent operating-system appearance signals without coupling Framework logic to platform APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/system_appearance_state.h"
int main(void) {
    UmiAppearanceSystemAppearanceState item;
    if (umi_appearance_system_appearance_state_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_system_appearance_state_is_valid(&item)) return 2;
    return 0;
}
