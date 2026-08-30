/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/integration_fabric_service.h
 *
 * PURPOSE:
 *   Compose canonical Integration Fabric registries, health and snapshots without replacing messaging/runtime implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_INTEGRATION_FABRIC_SERVICE_H
#define UMICOM_INTEGRATION_FABRIC_INTEGRATION_FABRIC_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/service_catalogue.h"
#include "umicom/integration/fabric/service_registry.h"
#include "umicom/integration/fabric/endpoint_registry.h"
#include "umicom/integration/fabric/connector_catalogue.h"
#include "umicom/integration/fabric/api_catalogue.h"
#include "umicom/integration/fabric/route_table.h"
#include "umicom/integration/fabric/fabric_health.h"
#include "umicom/integration/fabric/fabric_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiIntegrationFabricService { UmiFabricServiceCatalogue services; UmiFabricServiceRegistry instances; UmiFabricEndpointRegistry endpoints; UmiFabricConnectorCatalogue connectors; UmiFabricApiCatalogue apis; UmiFabricRouteTable routes; UmiFabricHealthSummary health; uint64_t revision; bool running; } UmiIntegrationFabricService;
void umi_integration_fabric_service_init(UmiIntegrationFabricService *service);
UmiStatus umi_integration_fabric_service_start(UmiIntegrationFabricService *service);
UmiStatus umi_integration_fabric_service_snapshot(const UmiIntegrationFabricService *service,UmiFabricSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
