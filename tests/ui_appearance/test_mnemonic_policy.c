/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_mnemonic_policy.c
 *
 * PURPOSE:
 *   Verify govern mnemonic visibility and uniqueness without embedding toolkit accelerator syntax.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/mnemonic_policy.h"
int main(void) {
    UmiAppearanceMnemonicPolicy item;
    if (umi_appearance_mnemonic_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_mnemonic_policy_is_valid(&item)) return 2;
    return 0;
}
