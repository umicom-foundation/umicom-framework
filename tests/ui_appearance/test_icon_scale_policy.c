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
int main(void) {
    UmiAppearanceIconScalePolicy item;
    if (umi_appearance_icon_scale_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_icon_scale_policy_is_valid(&item)) return 2;
    item.logical_size_dp=20.0; item.scale=1.5; if(umi_appearance_icon_scale_policy_resolve(&item)!=UMI_STATUS_OK || item.physical_size_px!=30U) return 3;
    return 0;
}
