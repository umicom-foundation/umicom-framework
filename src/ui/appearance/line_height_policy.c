/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/line_height_policy.c
 *
 * PURPOSE:
 *   Maintain readable line-height bounds as font and accessibility scale changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/line_height_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_line_height_policy_init(UmiAppearanceLineHeightPolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"line-height.default");
    item->minimum_multiplier=1.1;
    item->preferred_multiplier=1.4;
    item->maximum_multiplier=2.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_line_height_policy_is_valid(const UmiAppearanceLineHeightPolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_multiplier > 0.0 && item->preferred_multiplier >= item->minimum_multiplier && item->maximum_multiplier >= item->preferred_multiplier);
}
