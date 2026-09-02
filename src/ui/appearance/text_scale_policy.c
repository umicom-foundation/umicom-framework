/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/text_scale_policy.c
 *
 * PURPOSE:
 *   Clamp user text scaling while preserving semantic size hierarchy and accessibility intent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/text_scale_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_text_scale_policy_init(UmiAppearanceTextScalePolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"text-scale.default");
    item->minimum_scale=0.8;
    item->maximum_scale=3.0;
    item->requested_scale=1.0;
    item->resolved_scale=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_text_scale_policy_is_valid(const UmiAppearanceTextScalePolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_scale > 0.0 && item->maximum_scale >= item->minimum_scale);
}
/*
 * Provide the appearance text scale policy resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_text_scale_policy_resolve(UmiAppearanceTextScalePolicy *item,double requested){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;item->requested_scale=requested;return umi_appearance_clamp_scale(requested,item->minimum_scale,item->maximum_scale,&item->resolved_scale);}
