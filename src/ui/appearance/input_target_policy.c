/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/input_target_policy.c
 *
 * PURPOSE:
 *   Resolve minimum interactive target dimensions by pointer, touch, keyboard or hybrid modality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/input_target_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_input_target_policy_init(UmiAppearanceInputTargetPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"target.pointer");
    item->modality=UMI_APPEARANCE_INPUT_POINTER;
    item->minimum_width_dp=24.0;
    item->minimum_height_dp=24.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_input_target_policy_is_valid(const UmiAppearanceInputTargetPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_width_dp > 0.0 && item->minimum_height_dp > 0.0);
}
/*
 * Provide the appearance input target policy for modality operation used by this module
 * and its client applications.
 */
UmiStatus umi_appearance_input_target_policy_for_modality(UmiAppearanceInputTargetPolicy *item,UmiAppearanceInputModality modality){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;item->modality=modality;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(modality==UMI_APPEARANCE_INPUT_TOUCH||modality==UMI_APPEARANCE_INPUT_HYBRID){item->minimum_width_dp=44.0;item->minimum_height_dp=44.0;}/* Use this fallback path when the earlier condition does not apply. */ else{item->minimum_width_dp=24.0;item->minimum_height_dp=24.0;}return UMI_STATUS_OK;}
