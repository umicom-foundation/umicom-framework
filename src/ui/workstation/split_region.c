/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/split_region.c
 *
 * PURPOSE:
 *   Implement bounded split-region geometry for persistent two-pane workstation layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/split_region.h"

/*
 * Initialise ws split region from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_split_region_init(UmiWsSplitRegion *split,
                                   const char *split_id,
                                   UmiUiOrientation orientation,
                                   double ratio) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (split == NULL || !umi_ws_id_valid(split_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (orientation != UMI_UI_HORIZONTAL && orientation != UMI_UI_VERTICAL) return UMI_STATUS_INVALID_ARGUMENT;
    *split = (UmiWsSplitRegion){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(split->split_id, sizeof(split->split_id), split_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    split->orientation = orientation;
    split->minimum_ratio = 0.15;
    split->maximum_ratio = 0.85;
    split->ratio = umi_ws_clamp_unit(ratio);
    /* Apply this branch only when its contract condition is satisfied. */
    if (split->ratio < split->minimum_ratio) split->ratio = split->minimum_ratio;
    /* Apply this branch only when its contract condition is satisfied. */
    if (split->ratio > split->maximum_ratio) split->ratio = split->maximum_ratio;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws split region bind operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_split_region_bind(UmiWsSplitRegion *split,
                                   const char *first_id,
                                   const char *second_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (split == NULL || !umi_ws_id_valid(first_id) || !umi_ws_id_valid(second_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(split->first_id, sizeof(split->first_id), first_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ws_copy_text(split->second_id, sizeof(split->second_id), second_id);
}

/*
 * Provide the ws split region set ratio operation used by this module and its client
 * applications.
 */
double umi_ws_split_region_set_ratio(UmiWsSplitRegion *split, double ratio) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (split == NULL) return 0.0;
    split->ratio = ratio;
    /* Apply this branch only when its contract condition is satisfied. */
    if (split->ratio < split->minimum_ratio) split->ratio = split->minimum_ratio;
    /* Apply this branch only when its contract condition is satisfied. */
    if (split->ratio > split->maximum_ratio) split->ratio = split->maximum_ratio;
    return split->ratio;
}
