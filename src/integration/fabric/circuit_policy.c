/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/circuit_policy.c
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
#include "umicom/integration/fabric/circuit_policy.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric circuit policy default operation used by this module and its client
 * applications.
 */
void umi_fabric_circuit_policy_default(UmiFabricCircuitPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL){policy->failure_threshold=3U;policy->open_interval_ms=1000U;policy->half_open_successes=2U;}}
/*
 * Provide the fabric circuit record operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_circuit_record(const UmiFabricCircuitPolicy *p,UmiFabricCircuitEvidence *e,bool success,uint64_t now_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||e==NULL||p->failure_threshold==0U||p->half_open_successes==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->state==0)e->state=UMI_FABRIC_CIRCUIT_CLOSED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->state==UMI_FABRIC_CIRCUIT_OPEN){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(now_ms<e->opened_ms||now_ms-e->opened_ms<p->open_interval_ms)return UMI_STATUS_BUSY;e->state=UMI_FABRIC_CIRCUIT_HALF_OPEN;e->half_open_success_count=0U;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->state==UMI_FABRIC_CIRCUIT_HALF_OPEN){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(success){e->half_open_success_count++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->half_open_success_count>=p->half_open_successes){e->state=UMI_FABRIC_CIRCUIT_CLOSED;e->consecutive_failures=0U;}}/* Use this fallback path when the earlier condition does not apply. */ else{e->state=UMI_FABRIC_CIRCUIT_OPEN;e->opened_ms=now_ms;}return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(success)e->consecutive_failures=0U;/* Use this fallback path when the earlier condition does not apply. */ else{e->consecutive_failures++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->consecutive_failures>=p->failure_threshold){e->state=UMI_FABRIC_CIRCUIT_OPEN;e->opened_ms=now_ms;}}return UMI_STATUS_OK;}
