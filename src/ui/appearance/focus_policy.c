/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/focus_policy.c
 *
 * PURPOSE:
 *   Define visible keyboard-focus treatment requirements across all renderer adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_focus_policy_init(UmiAppearanceFocusPolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"focus.default");
    item->ring_width=2.0;
    item->ring_offset=2.0;
    item->always_visible_for_keyboard=true;
    item->clip_safe=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_focus_policy_is_valid(const UmiAppearanceFocusPolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->ring_width > 0.0 && item->ring_offset >= 0.0);
}
