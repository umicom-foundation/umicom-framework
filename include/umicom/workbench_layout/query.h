/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/query.h
 *
 * PURPOSE:
 *   Query semantic layout nodes by identity, component, application owner, context group, dock region, visibility and structural role.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_QUERY_H
#define UMICOM_WORKBENCH_LAYOUT_QUERY_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct UmiWorkbenchLayoutNodeQueryResult {
    size_t indices[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t count;
    bool truncated;
} UmiWorkbenchLayoutNodeQueryResult;

UmiWorkbenchLayoutNodeQuery
umi_workbench_layout_node_query_default(void);

bool umi_workbench_layout_node_matches_query(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutNodeQuery *query);

UmiStatus umi_workbench_layout_query_nodes(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    UmiWorkbenchLayoutNodeQueryResult *out_result);

UmiStatus umi_workbench_layout_query_first(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    const UmiWorkbenchLayoutNode **out_node);

size_t umi_workbench_layout_count_kind(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutNodeKind kind);

size_t umi_workbench_layout_count_owner(
    const UmiWorkbenchLayoutDocument *document,
    const char *owner_application_id);

size_t umi_workbench_layout_count_component(
    const UmiWorkbenchLayoutDocument *document,
    const char *component_id);

#ifdef __cplusplus
}
#endif

#endif
