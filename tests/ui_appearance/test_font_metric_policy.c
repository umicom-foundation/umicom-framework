/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_metric_policy.c
 *
 * PURPOSE:
 *   Verify define renderer-neutral font metric tolerances used to prevent clipping and layout drift.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_metric_policy.h"
int main(void) {
    UmiAppearanceFontMetricPolicy item;
    if (umi_appearance_font_metric_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_font_metric_policy_is_valid(&item)) return 2;
    return 0;
}
