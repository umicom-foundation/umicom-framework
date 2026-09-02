/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_system_appearance_state.c
 *
 * PURPOSE:
 *   Verify represent operating-system appearance signals without coupling Framework logic to platform APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/system_appearance_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceSystemAppearanceState item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_system_appearance_state_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_system_appearance_state_is_valid(&item)) return 2;
    return 0;
}
