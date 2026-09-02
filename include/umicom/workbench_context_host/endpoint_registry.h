/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/endpoint_registry.h
 *
 * PURPOSE:
 *   Maintain a bounded dynamic catalogue of context-aware workbench endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_REGISTRY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_REGISTRY_H

#include "umicom/workbench_context_host/endpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context host endpoint registry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostEndpointRegistry {
    UmiWorkbenchContextHostEndpoint *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostEndpointRegistry;

/**
 * Initialise workbench context host endpoint registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_endpoint_registry_init(
    UmiWorkbenchContextHostEndpointRegistry *registry);
/**
 * Release or reset state held by workbench context host endpoint registry so the same
 * storage can be reused safely.
 */
void umi_workbench_context_host_endpoint_registry_destroy(
    UmiWorkbenchContextHostEndpointRegistry *registry);
/**
 * Provide the workbench context host endpoint registry upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_endpoint_registry_upsert(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const UmiWorkbenchContextHostEndpoint *endpoint);
/**
 * Remove workbench context host endpoint registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_host_endpoint_registry_remove(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id);
/**
 * Find workbench context host endpoint registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id);
/**
 * Provide the workbench context host endpoint registry find const operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_const(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id);
/**
 * Provide the workbench context host endpoint registry find panel operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_panel(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *panel_id);
/**
 * Return the number of records represented by workbench context host endpoint registry
 * without changing their state.
 */
size_t umi_workbench_context_host_endpoint_registry_count(
    const UmiWorkbenchContextHostEndpointRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
