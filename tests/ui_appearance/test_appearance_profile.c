/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_profile.c
 *
 * PURPOSE:
 *   Verify capture a resolved user/application appearance profile shared by every renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceAppearanceProfile item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_profile_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_profile_is_valid(&item)) return 2;
    return 0;
}
