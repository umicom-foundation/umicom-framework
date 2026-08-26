/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/navigation_adaptation.h
 *
 * PURPOSE:
 *   Resolve navigation capacity, overflow and pattern for a concrete viewport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_NAVIGATION_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_NAVIGATION_ADAPTATION_H
#include "umicom/ui/adaptive/navigation_model.h"
#include "umicom/ui/adaptive/navigation_pattern.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveNavigationDecision {
    UmiAdaptiveNavigationPattern pattern;
    size_t visible_items;
    size_t overflow_items;
} UmiAdaptiveNavigationDecision;
/* Adapt a navigation model to size and input constraints. */
UmiStatus umi_adaptive_navigation_adaptation_resolve(const UmiAdaptiveNavigationModel *model,
                                                     UmiDesignSizeClass size_class,
                                                     UmiAdaptiveInputMode input_mode,
                                                     UmiAdaptiveNavigationDecision *out_decision);

#ifdef __cplusplus
}
#endif
#endif
