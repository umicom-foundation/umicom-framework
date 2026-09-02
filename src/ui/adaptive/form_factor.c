/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/form_factor.c
 *
 * PURPOSE:
 *   Classify viewport aspect ratios for adaptive shell and preview decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/form_factor.h"

/* Classify aspect ratio without relying on physical display technology. */
UmiStatus umi_adaptive_form_factor_classify(UmiAdaptiveViewport viewport,
                                            UmiAdaptiveFormFactor *out_factor)
{
    double ratio;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (viewport.width <= 0 || viewport.height <= 0 || out_factor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ratio = (double)viewport.width / (double)viewport.height;
    /* Apply this branch only when its contract condition is satisfied. */
    if (ratio < 0.85) {
        *out_factor = UMI_ADAPTIVE_FORM_TALL;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (ratio < 1.6) {
        *out_factor = UMI_ADAPTIVE_FORM_STANDARD;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (ratio < 2.2) {
        *out_factor = UMI_ADAPTIVE_FORM_WIDE;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        *out_factor = UMI_ADAPTIVE_FORM_ULTRAWIDE;
    }
    return UMI_STATUS_OK;
}
