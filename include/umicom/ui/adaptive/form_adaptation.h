/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/form_adaptation.h
 *
 * PURPOSE:
 *   Adapt form presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_FORM_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_FORM_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive form decision data shared with callers of this public contract.
 */
typedef struct UmiAdaptiveFormDecision { uint16_t columns; int labels_above; int full_width_actions; } UmiAdaptiveFormDecision;
/* Resolve form column and label layout for available size and touch input. */
UmiAdaptiveFormDecision umi_adaptive_form_adaptation_resolve(UmiDesignSizeClass size_class, UmiAdaptiveInputMode input_mode);

#ifdef __cplusplus
}
#endif
#endif
