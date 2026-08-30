/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/input_affordance_policy.c
 *
 * PURPOSE:
 *   Require hover, focus, pressed and touch feedback appropriate to available input modalities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/input_affordance_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_input_affordance_policy_init(UmiAppearanceInputAffordancePolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"affordance.hybrid");
    item->require_hover_feedback=true;
    item->require_focus_feedback=true;
    item->require_pressed_feedback=true;
    item->require_touch_feedback=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_input_affordance_policy_is_valid(const UmiAppearanceInputAffordancePolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->require_focus_feedback && item->require_pressed_feedback);
}
