/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/form_factor.c
 *
 * PURPOSE:
 *   Classify viewport aspect ratios for adaptive shell and preview decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/form_factor.h"

/* Classify aspect ratio without relying on physical display technology. */
UmiStatus umi_adaptive_form_factor_classify(UmiAdaptiveViewport viewport,
                                            UmiAdaptiveFormFactor *out_factor)
{
    double ratio;
    if (viewport.width <= 0 || viewport.height <= 0 || out_factor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ratio = (double)viewport.width / (double)viewport.height;
    if (ratio < 0.85) {
        *out_factor = UMI_ADAPTIVE_FORM_TALL;
    } else if (ratio < 1.6) {
        *out_factor = UMI_ADAPTIVE_FORM_STANDARD;
    } else if (ratio < 2.2) {
        *out_factor = UMI_ADAPTIVE_FORM_WIDE;
    } else {
        *out_factor = UMI_ADAPTIVE_FORM_ULTRAWIDE;
    }
    return UMI_STATUS_OK;
}
