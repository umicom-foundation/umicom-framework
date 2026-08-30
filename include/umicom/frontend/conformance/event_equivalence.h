/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/event_equivalence.h
 *
 * PURPOSE:
 *   event-family parity scoring for pointer, keyboard, input, focus and drag/drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_EVENT_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_EVENT_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcEventEquivalence { uint64_t required; uint64_t supported; } UmiFcEventEquivalence;
double umi_fc_event_equivalence_evaluate(const UmiFcEventEquivalence *item);
UmiFcOutcome umi_fc_event_equivalence_outcome(const UmiFcEventEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
