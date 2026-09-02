/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_scale_policy.c
 *
 * PURPOSE:
 *   Resolve logical icon size to physical pixels using the effective display scale.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_scale_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_icon_scale_policy_init(UmiAppearanceIconScalePolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"icon.default");
    item->logical_size_dp=16.0;
    item->scale=1.0;
    item->physical_size_px=16U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_icon_scale_policy_is_valid(const UmiAppearanceIconScalePolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->logical_size_dp > 0.0 && item->scale > 0.0 && item->physical_size_px > 0U);
}
#include <math.h>
/*
 * Provide the appearance icon scale policy resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_icon_scale_policy_resolve(UmiAppearanceIconScalePolicy *item){double pixels;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->logical_size_dp<=0.0||item->scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;pixels=item->logical_size_dp*item->scale;item->physical_size_px=(uint32_t)floor(pixels+0.5);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item->physical_size_px==0U)item->physical_size_px=1U;return UMI_STATUS_OK;}
