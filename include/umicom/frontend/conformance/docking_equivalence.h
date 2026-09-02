/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/docking_equivalence.h
 *
 * PURPOSE:
 *   docking-feature parity and responsive fallback scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_DOCKING_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_DOCKING_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc docking equivalence data shared with callers of this public contract.
 */
typedef struct UmiFcDockingEquivalence { uint64_t required; uint64_t supported; bool responsive_fallback; } UmiFcDockingEquivalence;
/**
 * Provide the fc docking equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_docking_equivalence_evaluate(const UmiFcDockingEquivalence *item);
/**
 * Provide the fc docking equivalence outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_docking_equivalence_outcome(const UmiFcDockingEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
