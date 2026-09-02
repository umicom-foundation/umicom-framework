/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/typography_policy.c
 *
 * PURPOSE:
 *   Govern semantic typography scaling, minimum readable text size and font smoothing intent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/typography_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_typography_policy_init(UmiAppearanceTypographyPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"typography.default");
    item->base_text_scale=1.0;
    item->minimum_text_dp=12.0;
    item->maximum_text_scale=3.0;
    item->respect_user_scale=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_typography_policy_is_valid(const UmiAppearanceTypographyPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->base_text_scale > 0.0 && item->minimum_text_dp > 0.0 && item->maximum_text_scale >= item->base_text_scale);
}
