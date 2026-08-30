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
typedef struct UmiHealthGate UmiHealthGate;
UmiStatus umi_health_gate_create(UmiHealthGate **out_gate);
void umi_health_gate_destroy(UmiHealthGate *gate);
UmiStatus umi_health_gate_require(UmiHealthGate *gate,
                                  const char *component_id,
                                  UmiHealthState minimum_state);
UmiStatus umi_health_gate_evaluate(const UmiHealthGate *gate,
                                   const UmiHealthRegistry *registry,
                                   char *out_reason,
                                   size_t reason_capacity);
size_t umi_health_gate_count(const UmiHealthGate *gate);
#ifdef __cplusplus
}
#endif

#endif
