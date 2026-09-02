/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/query.h
 *
 * PURPOSE:
 *   Query semantic layout nodes by identity, component, application owner, context group, dock region, visibility and structural role.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_QUERY_H
#define UMICOM_WORKBENCH_LAYOUT_QUERY_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout node query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutNodeQuery {
    uint32_t structure_size;
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char component_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char context_group_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutNodeKind kind;
    UmiWorkbenchLayoutDockRegion dock_region;
    UmiWorkbenchLayoutVisibility visibility;
    uint32_t required_flags;
    uint32_t prohibited_flags;
    bool include_containers;
    bool include_leaf_nodes;
} UmiWorkbenchLayoutNodeQuery;

/**
 * Represent the workbench layout node query result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutNodeQueryResult {
    size_t indices[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t count;
    bool truncated;
} UmiWorkbenchLayoutNodeQueryResult;

/**
 * Provide the workbench layout node query default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutNodeQuery
umi_workbench_layout_node_query_default(void);

/**
 * Provide the workbench layout node matches query operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_node_matches_query(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutNodeQuery *query);

/**
 * Provide the workbench layout query nodes operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_query_nodes(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    UmiWorkbenchLayoutNodeQueryResult *out_result);

/**
 * Provide the workbench layout query first operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_query_first(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    const UmiWorkbenchLayoutNode **out_node);

/**
 * Provide the workbench layout count kind operation used by this module and its client
 * applications.
 */
size_t umi_workbench_layout_count_kind(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutNodeKind kind);

/**
 * Provide the workbench layout count owner operation used by this module and its client
 * applications.
 */
size_t umi_workbench_layout_count_owner(
    const UmiWorkbenchLayoutDocument *document,
    const char *owner_application_id);

/**
 * Provide the workbench layout count component operation used by this module and its
 * client applications.
 */
size_t umi_workbench_layout_count_component(
    const UmiWorkbenchLayoutDocument *document,
    const char *component_id);

#ifdef __cplusplus
}
#endif

#endif
