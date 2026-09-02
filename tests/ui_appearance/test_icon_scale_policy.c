/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_scale_policy.c
 *
 * PURPOSE:
 *   Verify resolve logical icon size to physical pixels using the effective display scale.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_scale_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceIconScalePolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_icon_scale_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_icon_scale_policy_is_valid(&item)) return 2;
    item.logical_size_dp=20.0; item.scale=1.5; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_icon_scale_policy_resolve(&item)!=UMI_STATUS_OK || item.physical_size_px!=30U) return 3;
    return 0;
}
