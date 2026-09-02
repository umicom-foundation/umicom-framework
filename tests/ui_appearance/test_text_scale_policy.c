/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_text_scale_policy.c
 *
 * PURPOSE:
 *   Verify clamp user text scaling while preserving semantic size hierarchy and accessibility intent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/text_scale_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceTextScalePolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_text_scale_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_text_scale_policy_is_valid(&item)) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_text_scale_policy_resolve(&item,5.0)!=UMI_STATUS_OK || item.resolved_scale!=3.0) return 3;
    return 0;
}
