/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/routing_decision.h
 *
 * PURPOSE:
 *   Capture immutable route-selection evidence for audit and diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_ROUTING_DECISION_H
#define UMICOM_INTEGRATION_FABRIC_ROUTING_DECISION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/route_rule.h"
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric routing decision data shared with callers of this public contract.
 */
typedef struct UmiFabricRoutingDecision { char route_id[UMI_FABRIC_ID_CAPACITY]; char destination_id[UMI_FABRIC_ID_CAPACITY]; char reason[UMI_FABRIC_TEXT_CAPACITY]; uint64_t decided_ms; bool fallback; } UmiFabricRoutingDecision;
/**
 * Initialise fabric routing decision from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_routing_decision_init(UmiFabricRoutingDecision *decision,const UmiFabricRouteRule *rule,const char *reason,uint64_t decided_ms,bool fallback);

#ifdef __cplusplus
}
#endif
#endif
