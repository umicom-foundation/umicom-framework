/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/fabric_health.h
 *
 * PURPOSE:
 *   Aggregate service, connector, routing and workflow state into an overall Fabric readiness classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_FABRIC_HEALTH_H
#define UMICOM_INTEGRATION_FABRIC_FABRIC_HEALTH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricHealthSummary { UmiFabricState service_state; UmiFabricState connector_state; UmiFabricState routing_state; UmiFabricState workflow_state; UmiFabricState overall_state; size_t blockers; } UmiFabricHealthSummary;
void umi_fabric_health_evaluate(UmiFabricHealthSummary *summary);

#ifdef __cplusplus
}
#endif
#endif
