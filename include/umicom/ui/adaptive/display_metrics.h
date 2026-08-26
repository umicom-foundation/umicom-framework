/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/display_metrics.h
 *
 * PURPOSE:
 *   Convert physical pixel metrics and scale factors into stable logical viewport dimensions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_DISPLAY_METRICS_H
#define UMICOM_UI_ADAPTIVE_DISPLAY_METRICS_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveDisplayMetrics {
    int32_t pixel_width;
    int32_t pixel_height;
    uint32_t dpi;
    double scale_factor;
} UmiAdaptiveDisplayMetrics;
/* Initialise validated physical display metrics. */
UmiStatus umi_adaptive_display_metrics_init(UmiAdaptiveDisplayMetrics *metrics,
                                            int32_t pixel_width,
                                            int32_t pixel_height,
                                            uint32_t dpi,
                                            double scale_factor);
/* Convert physical metrics into logical renderer-independent viewport dimensions. */
UmiStatus umi_adaptive_display_metrics_logical_viewport(const UmiAdaptiveDisplayMetrics *metrics,
                                                        UmiAdaptiveViewport *out_viewport);

#ifdef __cplusplus
}
#endif
#endif
