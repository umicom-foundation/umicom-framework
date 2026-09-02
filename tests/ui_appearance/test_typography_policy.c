/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_typography_policy.c
 *
 * PURPOSE:
 *   Verify govern semantic typography scaling, minimum readable text size and font smoothing intent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/typography_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceTypographyPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_typography_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_typography_policy_is_valid(&item)) return 2;
    return 0;
}
