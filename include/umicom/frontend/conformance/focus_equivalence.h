/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/focus_equivalence.h
 *
 * PURPOSE:
 *   focus traversal equivalence scoring across native and browser renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_FOCUS_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_FOCUS_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcFocusEquivalence { size_t expected_order; size_t actual_order; size_t traps_missing; } UmiFcFocusEquivalence;
double umi_fc_focus_equivalence_evaluate(const UmiFcFocusEquivalence *item);
bool umi_fc_focus_equivalence_passes(const UmiFcFocusEquivalence *item,double minimum);

#ifdef __cplusplus
}
#endif
#endif
