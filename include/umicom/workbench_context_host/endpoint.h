/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/endpoint.h
 *
 * PURPOSE:
 *   Describe a context-aware workbench panel endpoint and its accepted/published context capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_H

#include "umicom/workbench_context_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context host endpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostEndpoint {
    uint32_t structure_size;
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostPanelRole role;
    UmiWorkbenchContextHostEndpointState state;
    UmiWorkbenchContextLinkMode mode;
    uint64_t accepted_kinds_mask;
    uint64_t published_kinds_mask;
    uint64_t delivery_count;
    uint64_t publish_count;
    uint64_t revision;
    bool enabled;
} UmiWorkbenchContextHostEndpoint;

/**
 * Initialise workbench context host endpoint from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_endpoint_init(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *endpoint_id);
/**
 * Check that workbench context host endpoint satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_host_endpoint_validate(
    const UmiWorkbenchContextHostEndpoint *endpoint);
/**
 * Provide the workbench context host endpoint set identity operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_endpoint_set_identity(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *panel_id,
    const char *application_id,
    const char *display_name);
/**
 * Provide the workbench context host endpoint set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_endpoint_set_group(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *group_id,
    UmiWorkbenchContextLinkMode mode);
/**
 * Provide the workbench context host endpoint accepts operation used by this module and
 * its client applications.
 */
bool umi_workbench_context_host_endpoint_accepts(
    const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind kind);
/**
 * Provide the workbench context host endpoint publishes operation used by this module and
 * its client applications.
 */
bool umi_workbench_context_host_endpoint_publishes(
    const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind kind);

#ifdef __cplusplus
}
#endif
#endif
