/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/circuit_policy.h
 *
 * PURPOSE:
 *   Model circuit-breaker transitions from failure evidence without replacing the canonical resilience subsystem.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_CIRCUIT_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_CIRCUIT_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric circuit policy data shared with callers of this public contract.
 */
typedef struct UmiFabricCircuitPolicy { uint32_t failure_threshold; uint64_t open_interval_ms; uint32_t half_open_successes; } UmiFabricCircuitPolicy;
/**
 * Represent the fabric circuit evidence data shared with callers of this public contract.
 */
typedef struct UmiFabricCircuitEvidence { UmiFabricCircuitState state; uint32_t consecutive_failures; uint32_t half_open_success_count; uint64_t opened_ms; } UmiFabricCircuitEvidence;
/**
 * Provide the fabric circuit policy default operation used by this module and its client
 * applications.
 */
void umi_fabric_circuit_policy_default(UmiFabricCircuitPolicy *policy);
/**
 * Provide the fabric circuit record operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_circuit_record(const UmiFabricCircuitPolicy *policy,UmiFabricCircuitEvidence *evidence,bool success,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
