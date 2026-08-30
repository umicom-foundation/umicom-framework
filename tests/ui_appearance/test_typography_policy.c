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
int main(void) {
    UmiAppearanceTypographyPolicy item;
    if (umi_appearance_typography_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_typography_policy_is_valid(&item)) return 2;
    return 0;
}
