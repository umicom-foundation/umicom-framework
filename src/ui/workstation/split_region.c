/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/split_region.c
 *
 * PURPOSE:
 *   Implement bounded split-region geometry for persistent two-pane workstation layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/split_region.h"

UmiStatus umi_ws_split_region_init(UmiWsSplitRegion *split,
                                   const char *split_id,
                                   UmiUiOrientation orientation,
                                   double ratio) {
    if (split == NULL || !umi_ws_id_valid(split_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (orientation != UMI_UI_HORIZONTAL && orientation != UMI_UI_VERTICAL) return UMI_STATUS_INVALID_ARGUMENT;
    *split = (UmiWsSplitRegion){0};
    if (umi_ws_copy_text(split->split_id, sizeof(split->split_id), split_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    split->orientation = orientation;
    split->minimum_ratio = 0.15;
    split->maximum_ratio = 0.85;
    split->ratio = umi_ws_clamp_unit(ratio);
    if (split->ratio < split->minimum_ratio) split->ratio = split->minimum_ratio;
    if (split->ratio > split->maximum_ratio) split->ratio = split->maximum_ratio;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_split_region_bind(UmiWsSplitRegion *split,
                                   const char *first_id,
                                   const char *second_id) {
    if (split == NULL || !umi_ws_id_valid(first_id) || !umi_ws_id_valid(second_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_ws_copy_text(split->first_id, sizeof(split->first_id), first_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ws_copy_text(split->second_id, sizeof(split->second_id), second_id);
}

double umi_ws_split_region_set_ratio(UmiWsSplitRegion *split, double ratio) {
    if (split == NULL) return 0.0;
    split->ratio = ratio;
    if (split->ratio < split->minimum_ratio) split->ratio = split->minimum_ratio;
    if (split->ratio > split->maximum_ratio) split->ratio = split->maximum_ratio;
    return split->ratio;
}
