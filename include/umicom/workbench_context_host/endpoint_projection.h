/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/endpoint_projection.h
 *
 * PURPOSE:
 *   Project registered context endpoints into a stable panel-link inventory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_ENDPOINT_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINT_PROJECTION_ROWS 256U
/**
 * Represent the workbench context host endpoint projection row data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextHostEndpointProjectionRow {
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostPanelRole role;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextHostEndpointState state;
    size_t queued_delivery_count;
    uint64_t delivery_count;
    uint64_t publish_count;
} UmiWorkbenchContextHostEndpointProjectionRow;
/**
 * Represent the workbench context host endpoint projection data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextHostEndpointProjection {
    UmiWorkbenchContextHostEndpointProjectionRow rows[
        UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINT_PROJECTION_ROWS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchContextHostEndpointProjection;
/**
 * Provide the workbench context host endpoint projection build operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_endpoint_projection_build(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostEndpointProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
