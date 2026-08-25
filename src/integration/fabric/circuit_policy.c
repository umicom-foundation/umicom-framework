/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/circuit_policy.c
 *
 * PURPOSE:
 *   Model circuit-breaker transitions from failure evidence without replacing the canonical resilience subsystem.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/circuit_policy.h"
#include <string.h>
#include <limits.h>


void umi_fabric_circuit_policy_default(UmiFabricCircuitPolicy *policy){if(policy!=NULL){policy->failure_threshold=3U;policy->open_interval_ms=1000U;policy->half_open_successes=2U;}}
UmiStatus umi_fabric_circuit_record(const UmiFabricCircuitPolicy *p,UmiFabricCircuitEvidence *e,bool success,uint64_t now_ms){if(p==NULL||e==NULL||p->failure_threshold==0U||p->half_open_successes==0U)return UMI_STATUS_INVALID_ARGUMENT;if(e->state==0)e->state=UMI_FABRIC_CIRCUIT_CLOSED;if(e->state==UMI_FABRIC_CIRCUIT_OPEN){if(now_ms<e->opened_ms||now_ms-e->opened_ms<p->open_interval_ms)return UMI_STATUS_BUSY;e->state=UMI_FABRIC_CIRCUIT_HALF_OPEN;e->half_open_success_count=0U;}if(e->state==UMI_FABRIC_CIRCUIT_HALF_OPEN){if(success){e->half_open_success_count++;if(e->half_open_success_count>=p->half_open_successes){e->state=UMI_FABRIC_CIRCUIT_CLOSED;e->consecutive_failures=0U;}}else{e->state=UMI_FABRIC_CIRCUIT_OPEN;e->opened_ms=now_ms;}return UMI_STATUS_OK;}if(success)e->consecutive_failures=0U;else{e->consecutive_failures++;if(e->consecutive_failures>=p->failure_threshold){e->state=UMI_FABRIC_CIRCUIT_OPEN;e->opened_ms=now_ms;}}return UMI_STATUS_OK;}
