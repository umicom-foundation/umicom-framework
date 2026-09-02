/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_display_scale_profile.c
 *
 * PURPOSE:
 *   Verify combine display DPI, operating-system scale and user accessibility scale into one profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/display_scale_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceDisplayScaleProfile item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_display_scale_profile_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_display_scale_profile_is_valid(&item)) return 2;
    item.os_scale=1.5; item.user_scale=2.0; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_display_scale_profile_resolve(&item)!=UMI_STATUS_OK || item.effective_scale!=3.0) return 3;
    return 0;
}
