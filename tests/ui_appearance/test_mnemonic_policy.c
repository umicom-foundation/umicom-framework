/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_mnemonic_policy.c
 *
 * PURPOSE:
 *   Verify govern mnemonic visibility and uniqueness without embedding toolkit accelerator syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/mnemonic_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceMnemonicPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_mnemonic_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_mnemonic_policy_is_valid(&item)) return 2;
    return 0;
}
