/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_contrast_policy.c
 *
 * PURPOSE:
 *   Verify define certification thresholds for normal text, large text, icons and focus indicators.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_policy.h"
int main(void) {
    UmiAppearanceContrastPolicy item;
    if (umi_appearance_contrast_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_contrast_policy_is_valid(&item)) return 2;
    return 0;
}
