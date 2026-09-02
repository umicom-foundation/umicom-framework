/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/health_gate.h
 *
 * PURPOSE:
 *   Gate consequential operations on required Framework health components and
 *   minimum accepted states.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_HEALTH_GATE_H
#define UMICOM_RESILIENCE_HEALTH_GATE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/runtime/health.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_HEALTH_GATE_MAX 64U
/**
 * Represent the health gate data shared with callers of this public contract.
 */
typedef struct UmiHealthGate UmiHealthGate;
/**
 * Initialise health gate from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_health_gate_create(UmiHealthGate **out_gate);
/**
 * Release or reset state held by health gate so the same storage can be reused safely.
 */
void umi_health_gate_destroy(UmiHealthGate *gate);
/**
 * Provide the health gate require operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_gate_require(UmiHealthGate *gate,
                                  const char *component_id,
                                  UmiHealthState minimum_state);
/**
 * Provide the health gate evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_gate_evaluate(const UmiHealthGate *gate,
                                   const UmiHealthRegistry *registry,
                                   char *out_reason,
                                   size_t reason_capacity);
/**
 * Return the number of records represented by health gate without changing their state.
 */
size_t umi_health_gate_count(const UmiHealthGate *gate);
#ifdef __cplusplus
}
#endif

#endif
