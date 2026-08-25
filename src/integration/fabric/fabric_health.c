/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/fabric_health.c
 *
 * PURPOSE:
 *   Aggregate service, connector, routing and workflow state into an overall Fabric readiness classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/fabric_health.h"
#include <string.h>
#include <limits.h>


void umi_fabric_health_evaluate(UmiFabricHealthSummary *s){UmiFabricState states[4];size_t i;if(s==NULL)return;states[0]=s->service_state;states[1]=s->connector_state;states[2]=s->routing_state;states[3]=s->workflow_state;s->overall_state=UMI_FABRIC_STATE_READY;s->blockers=0U;for(i=0U;i<4U;++i){if(states[i]==UMI_FABRIC_STATE_FAILED||states[i]==UMI_FABRIC_STATE_BLOCKED){s->blockers++;s->overall_state=UMI_FABRIC_STATE_BLOCKED;}else if(states[i]==UMI_FABRIC_STATE_DEGRADED&&s->overall_state==UMI_FABRIC_STATE_READY)s->overall_state=UMI_FABRIC_STATE_DEGRADED;else if(states[i]==UMI_FABRIC_STATE_UNKNOWN&&s->overall_state==UMI_FABRIC_STATE_READY)s->overall_state=UMI_FABRIC_STATE_UNKNOWN;}}
