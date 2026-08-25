/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_health.c
 *
 * PURPOSE:
 *   Project service success/failure evidence into a stable health state without performing probes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_health.h"
#include <string.h>
#include <limits.h>


void umi_fabric_service_health_init(UmiFabricServiceHealth *health) { if(health!=NULL){(void)memset(health,0,sizeof(*health)); health->state=UMI_FABRIC_STATE_UNKNOWN;} }
void umi_fabric_service_health_record(UmiFabricServiceHealth *health,bool success,uint64_t now_ms,uint32_t failure_threshold) {
    if(health==NULL) return;
    if(success){health->successes++; health->consecutive_failures=0U; health->last_success_ms=now_ms; health->state=UMI_FABRIC_STATE_READY;}
    else {health->failures++; if(health->consecutive_failures<UINT32_MAX)health->consecutive_failures++; health->last_failure_ms=now_ms; health->state=(failure_threshold>0U && health->consecutive_failures>=failure_threshold)?UMI_FABRIC_STATE_FAILED:UMI_FABRIC_STATE_DEGRADED;}
}
