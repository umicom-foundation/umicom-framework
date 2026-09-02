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

/**
 * Represent the fc event equivalence data shared with callers of this public contract.
 */
typedef struct UmiFcEventEquivalence { uint64_t required; uint64_t supported; } UmiFcEventEquivalence;
/**
 * Provide the fc event equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_event_equivalence_evaluate(const UmiFcEventEquivalence *item);
/**
 * Provide the fc event equivalence outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_event_equivalence_outcome(const UmiFcEventEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
