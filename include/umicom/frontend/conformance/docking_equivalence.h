/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/docking_equivalence.h
 *
 * PURPOSE:
 *   docking-feature parity and responsive fallback scoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcDockingEquivalence { uint64_t required; uint64_t supported; bool responsive_fallback; } UmiFcDockingEquivalence;
double umi_fc_docking_equivalence_evaluate(const UmiFcDockingEquivalence *item);
UmiFcOutcome umi_fc_docking_equivalence_outcome(const UmiFcDockingEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
