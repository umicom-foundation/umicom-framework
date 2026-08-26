/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/form_factor.h
 *
 * PURPOSE:
 *   Classify viewport aspect ratios for adaptive shell and preview decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_FORM_FACTOR_H
#define UMICOM_UI_ADAPTIVE_FORM_FACTOR_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Classify a positive logical viewport into a stable aspect-ratio form factor. */
UmiStatus umi_adaptive_form_factor_classify(UmiAdaptiveViewport viewport,
                                            UmiAdaptiveFormFactor *out_factor);

#ifdef __cplusplus
}
#endif
#endif
