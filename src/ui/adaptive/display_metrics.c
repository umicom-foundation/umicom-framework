/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/display_metrics.c
 *
 * PURPOSE:
 *   Convert physical pixel metrics and scale factors into stable logical viewport dimensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/display_metrics.h"

/* Store physical metrics only when scale and dimensions are meaningful. */
UmiStatus umi_adaptive_display_metrics_init(UmiAdaptiveDisplayMetrics *metrics,
                                            int32_t pixel_width,
                                            int32_t pixel_height,
                                            uint32_t dpi,
                                            double scale_factor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL || pixel_width <= 0 || pixel_height <= 0 || dpi == 0U ||
        !(scale_factor > 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    metrics->pixel_width = pixel_width;
    metrics->pixel_height = pixel_height;
    metrics->dpi = dpi;
    metrics->scale_factor = scale_factor;
    return UMI_STATUS_OK;
}

/* Convert pixels to logical units using the discovered frontend scale factor. */
UmiStatus umi_adaptive_display_metrics_logical_viewport(const UmiAdaptiveDisplayMetrics *metrics,
                                                        UmiAdaptiveViewport *out_viewport)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL || out_viewport == NULL || !(metrics->scale_factor > 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_viewport->width = (int32_t)((double)metrics->pixel_width / metrics->scale_factor);
    out_viewport->height = (int32_t)((double)metrics->pixel_height / metrics->scale_factor);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (out_viewport->width <= 0 || out_viewport->height <= 0) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
