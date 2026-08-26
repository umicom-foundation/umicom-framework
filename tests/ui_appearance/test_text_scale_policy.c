/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_text_scale_policy.c
 *
 * PURPOSE:
 *   Verify clamp user text scaling while preserving semantic size hierarchy and accessibility intent.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/text_scale_policy.h"
int main(void) {
    UmiAppearanceTextScalePolicy item;
    if (umi_appearance_text_scale_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_text_scale_policy_is_valid(&item)) return 2;
    if (umi_appearance_text_scale_policy_resolve(&item,5.0)!=UMI_STATUS_OK || item.resolved_scale!=3.0) return 3;
    return 0;
}
