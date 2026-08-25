/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/selection_equivalence.h
 *
 * PURPOSE:
 *   selection-mode parity scoring and graceful-degradation classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcSelectionEquivalence { uint64_t required; uint64_t supported; } UmiFcSelectionEquivalence;
double umi_fc_selection_equivalence_evaluate(const UmiFcSelectionEquivalence *item);
UmiFcOutcome umi_fc_selection_equivalence_outcome(const UmiFcSelectionEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
