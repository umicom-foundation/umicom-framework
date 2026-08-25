/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/routing_decision.c
 *
 * PURPOSE:
 *   Capture immutable route-selection evidence for audit and diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/routing_decision.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_routing_decision_init(UmiFabricRoutingDecision *decision,const UmiFabricRouteRule *rule,const char *reason,uint64_t decided_ms,bool fallback){UmiStatus s;if(decision==NULL||rule==NULL||reason==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(decision,0,sizeof(*decision));s=umi_fabric_copy_text(decision->route_id,sizeof(decision->route_id),rule->route_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(decision->destination_id,sizeof(decision->destination_id),rule->destination_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(decision->reason,sizeof(decision->reason),reason);if(s!=UMI_STATUS_OK)return s;decision->decided_ms=decided_ms;decision->fallback=fallback;return UMI_STATUS_OK;}
