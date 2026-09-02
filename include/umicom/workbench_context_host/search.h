/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/search.h
 *
 * PURPOSE:
 *   Search registered context endpoints by identity, application, role, group and display label.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SEARCH_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SEARCH_H
#include "umicom/workbench_context_host/endpoint_registry.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_SEARCH_RESULTS 128U
/**
 * Represent the workbench context host search result data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostSearchResult {
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostPanelRole role;
    int score;
} UmiWorkbenchContextHostSearchResult;
/**
 * Represent the workbench context host search results data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostSearchResults {
    UmiWorkbenchContextHostSearchResult items[UMI_WORKBENCH_CONTEXT_HOST_MAX_SEARCH_RESULTS];
    size_t count;
} UmiWorkbenchContextHostSearchResults;
/**
 * Provide the workbench context host search operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_search(
    const UmiWorkbenchContextHostEndpointRegistry *registry,const char *query,
    UmiWorkbenchContextHostSearchResults *out_results);
#ifdef __cplusplus
}
#endif
#endif
