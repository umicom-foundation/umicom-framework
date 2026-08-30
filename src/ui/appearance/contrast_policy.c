/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/contrast_policy.c
 *
 * PURPOSE:
 *   Define certification thresholds for normal text, large text, icons and focus indicators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_contrast_policy_init(UmiAppearanceContrastPolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"contrast.standard");
    item->normal_text_ratio=4.5;
    item->large_text_ratio=3.0;
    item->non_text_ratio=3.0;
    item->focus_ratio=3.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_contrast_policy_is_valid(const UmiAppearanceContrastPolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->normal_text_ratio >= 1.0 && item->large_text_ratio >= 1.0 && item->non_text_ratio >= 1.0);
}
