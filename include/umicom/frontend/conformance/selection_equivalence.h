/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/selection_equivalence.h
 *
 * PURPOSE:
 *   selection-mode parity scoring and graceful-degradation classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SELECTION_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_SELECTION_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc selection equivalence data shared with callers of this public contract.
 */
typedef struct UmiFcSelectionEquivalence { uint64_t required; uint64_t supported; } UmiFcSelectionEquivalence;
/**
 * Provide the fc selection equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_selection_equivalence_evaluate(const UmiFcSelectionEquivalence *item);
/**
 * Provide the fc selection equivalence outcome operation used by this module and its
 * client applications.
 */
UmiFcOutcome umi_fc_selection_equivalence_outcome(const UmiFcSelectionEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
