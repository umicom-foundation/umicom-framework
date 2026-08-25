/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/resize_policy.c
 *
 * PURPOSE:
 *   Implement minimum, maximum and preferred sizing policy for dense professional workstation panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/resize_policy.h"

UmiStatus umi_ws_resize_policy_init(UmiWsResizePolicy *policy,
                                    UmiUiSize minimum,
                                    UmiUiSize preferred,
                                    UmiUiSize maximum) {
    if (policy == NULL || minimum.width < 0 || minimum.height < 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (maximum.width < minimum.width || maximum.height < minimum.height) return UMI_STATUS_INVALID_ARGUMENT;
    *policy = (UmiWsResizePolicy){ minimum, preferred, maximum };
    policy->preferred.width = umi_ws_clamp_i32(preferred.width, minimum.width, maximum.width);
    policy->preferred.height = umi_ws_clamp_i32(preferred.height, minimum.height, maximum.height);
    return UMI_STATUS_OK;
}

UmiUiSize umi_ws_resize_policy_apply(const UmiWsResizePolicy *policy, UmiUiSize requested) {
    UmiUiSize result = requested;
    if (policy == NULL) return result;
    result.width = umi_ws_clamp_i32(requested.width, policy->minimum.width, policy->maximum.width);
    result.height = umi_ws_clamp_i32(requested.height, policy->minimum.height, policy->maximum.height);
    return result;
}
