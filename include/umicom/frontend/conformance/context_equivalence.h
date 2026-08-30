/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/context_equivalence.h
 *
 * PURPOSE:
 *   context propagation parity and directionality scoring across frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONTEXT_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_CONTEXT_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcContextEquivalence { uint64_t required; uint64_t supported; size_t direction_mismatches; } UmiFcContextEquivalence;
double umi_fc_context_equivalence_evaluate(const UmiFcContextEquivalence *item);
UmiFcOutcome umi_fc_context_equivalence_outcome(const UmiFcContextEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
