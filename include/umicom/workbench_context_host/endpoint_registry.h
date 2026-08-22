/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/endpoint_registry.h
 *
 * PURPOSE:
 *   Maintain a bounded dynamic catalogue of context-aware workbench endpoints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_REGISTRY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_REGISTRY_H

#include "umicom/workbench_context_host/endpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextHostEndpointRegistry {
    UmiWorkbenchContextHostEndpoint *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostEndpointRegistry;

void umi_workbench_context_host_endpoint_registry_init(
    UmiWorkbenchContextHostEndpointRegistry *registry);
void umi_workbench_context_host_endpoint_registry_destroy(
    UmiWorkbenchContextHostEndpointRegistry *registry);
UmiStatus umi_workbench_context_host_endpoint_registry_upsert(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const UmiWorkbenchContextHostEndpoint *endpoint);
UmiStatus umi_workbench_context_host_endpoint_registry_remove(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id);
UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id);
const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_const(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id);
const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_panel(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *panel_id);
size_t umi_workbench_context_host_endpoint_registry_count(
    const UmiWorkbenchContextHostEndpointRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
