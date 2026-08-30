/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/safe_area.c
 *
 * PURPOSE:
 *   Resolve platform safe-area insets into renderer-neutral logical content bounds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/safe_area.h"

/* Safe areas must never consume negative space. */
int umi_adaptive_safe_area_valid(UmiAdaptiveInsets insets)
{
    return insets.top >= 0 && insets.right >= 0 && insets.bottom >= 0 && insets.left >= 0;
}

/* Subtract safe-area insets while rejecting impossible or inverted content bounds. */
UmiStatus umi_adaptive_safe_area_content_rect(UmiAdaptiveViewport viewport,
                                              UmiAdaptiveInsets insets,
                                              UmiUiRect *out_rect)
{
    int32_t width;
    int32_t height;
    if (out_rect == NULL || viewport.width <= 0 || viewport.height <= 0 ||
        !umi_adaptive_safe_area_valid(insets)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    width = viewport.width - insets.left - insets.right;
    height = viewport.height - insets.top - insets.bottom;
    if (width <= 0 || height <= 0) {
        return UMI_STATUS_INVALID_STATE;
    }
    out_rect->x = insets.left;
    out_rect->y = insets.top;
    out_rect->width = width;
    out_rect->height = height;
    return UMI_STATUS_OK;
}
