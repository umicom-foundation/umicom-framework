/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_health.c
 *
 * PURPOSE:
 *   Project service success/failure evidence into a stable health state without performing probes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_health.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric service health from caller-provided values so later operations receive
 * a known state.
 */
void umi_fabric_service_health_init(UmiFabricServiceHealth *health) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(health!=NULL){(void)memset(health,0,sizeof(*health)); health->state=UMI_FABRIC_STATE_UNKNOWN;} }
/*
 * Provide the fabric service health record operation used by this module and its client
 * applications.
 */
void umi_fabric_service_health_record(UmiFabricServiceHealth *health,bool success,uint64_t now_ms,uint32_t failure_threshold) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(health==NULL) return;
    /* Apply this operation only while the related capability or state is available. */
    if(success){health->successes++; health->consecutive_failures=0U; health->last_success_ms=now_ms; health->state=UMI_FABRIC_STATE_READY;}
    /* Use this fallback path when the earlier condition does not apply. */
    else {health->failures++; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(health->consecutive_failures<UINT32_MAX)health->consecutive_failures++; health->last_failure_ms=now_ms; health->state=(failure_threshold>0U && health->consecutive_failures>=failure_threshold)?UMI_FABRIC_STATE_FAILED:UMI_FABRIC_STATE_DEGRADED;}
}
