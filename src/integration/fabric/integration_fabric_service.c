/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/integration_fabric_service.c
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
#include <stdint.h>
#include "umicom/integration/fabric/integration_fabric_service.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise integration fabric service from caller-provided values so later operations
 * receive a known state.
 */
void umi_integration_fabric_service_init(UmiIntegrationFabricService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;(void)memset(s,0,sizeof(*s));umi_fabric_service_catalogue_init(&s->services);umi_fabric_service_registry_init(&s->instances);umi_fabric_endpoint_registry_init(&s->endpoints);umi_fabric_connector_catalogue_init(&s->connectors);umi_fabric_api_catalogue_init(&s->apis);umi_fabric_route_table_init(&s->routes);s->health.service_state=UMI_FABRIC_STATE_UNKNOWN;s->health.connector_state=UMI_FABRIC_STATE_UNKNOWN;s->health.routing_state=UMI_FABRIC_STATE_UNKNOWN;s->health.workflow_state=UMI_FABRIC_STATE_UNKNOWN;umi_fabric_health_evaluate(&s->health);}
/*
 * Provide the integration fabric service start operation used by this module and its
 * client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_integration_fabric_service_start(UmiIntegrationFabricService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->running)return UMI_STATUS_ALREADY_EXISTS;s->running=true;s->revision++;s->health.service_state=UMI_FABRIC_STATE_READY;s->health.connector_state=UMI_FABRIC_STATE_READY;s->health.routing_state=UMI_FABRIC_STATE_READY;s->health.workflow_state=UMI_FABRIC_STATE_READY;umi_fabric_health_evaluate(&s->health);return UMI_STATUS_OK;}
#endif

UmiStatus umi_integration_fabric_service_start(UmiIntegrationFabricService *s){
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (s != NULL && s->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->running)return UMI_STATUS_ALREADY_EXISTS;s->running=true;s->revision++;s->health.service_state=UMI_FABRIC_STATE_READY;s->health.connector_state=UMI_FABRIC_STATE_READY;s->health.routing_state=UMI_FABRIC_STATE_READY;s->health.workflow_state=UMI_FABRIC_STATE_READY;umi_fabric_health_evaluate(&s->health);return UMI_STATUS_OK;}
/*
 * Provide the integration fabric service snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_fabric_service_snapshot(const UmiIntegrationFabricService *s,UmiFabricSnapshot *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_fabric_snapshot_build(s->services.count,s->instances.count,s->endpoints.count,s->connectors.count,s->apis.count,s->routes.count,s->health.overall_state,s->revision,out);}
