/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/orientation_change.c
 *
 * PURPOSE:
 *   Represent and apply deterministic orientation changes to logical viewport and safe-area state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/orientation_change.h"

/* Rotate logical geometry while keeping safe-area edges attached to physical edges. */
UmiStatus umi_adaptive_orientation_change_rotate(UmiAdaptiveViewport viewport,
                                                 UmiAdaptiveInsets safe_area,
                                                 int clockwise,
                                                 UmiAdaptiveOrientationChange *out_change)
{
    UmiAdaptiveInsets rotated;
    if (out_change == NULL || viewport.width <= 0 || viewport.height <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    out_change->from = umi_adaptive_orientation_from_viewport(viewport);
    out_change->viewport.width = viewport.height;
    out_change->viewport.height = viewport.width;
    if (clockwise != 0) {
        rotated.top = safe_area.left;
        rotated.right = safe_area.top;
        rotated.bottom = safe_area.right;
        rotated.left = safe_area.bottom;
    } else {
        rotated.top = safe_area.right;
        rotated.right = safe_area.bottom;
        rotated.bottom = safe_area.left;
        rotated.left = safe_area.top;
    }
    out_change->safe_area = rotated;
    out_change->to = umi_adaptive_orientation_from_viewport(out_change->viewport);
    return UMI_STATUS_OK;
}
