/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/focus_equivalence.h
 *
 * PURPOSE:
 *   focus traversal equivalence scoring across native and browser renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the fc focus equivalence data shared with callers of this public contract.
 */
typedef struct UmiFcFocusEquivalence { size_t expected_order; size_t actual_order; size_t traps_missing; } UmiFcFocusEquivalence;
/**
 * Provide the fc focus equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_focus_equivalence_evaluate(const UmiFcFocusEquivalence *item);
/**
 * Provide the fc focus equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_focus_equivalence_passes(const UmiFcFocusEquivalence *item,double minimum);

#ifdef __cplusplus
}
#endif
#endif
