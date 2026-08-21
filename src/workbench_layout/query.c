/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/query.c
 *
 * PURPOSE:
 *   Implement bounded semantic node queries for layout browsers, designers, workbench composition and architecture validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/query.h"

#include <string.h>

#include "internal.h"

UmiWorkbenchLayoutNodeQuery
umi_workbench_layout_node_query_default(void)
{
    UmiWorkbenchLayoutNodeQuery query;
    (void)memset(&query, 0, sizeof(query));
    query.structure_size = sizeof(query);
    query.include_containers = true;
    query.include_leaf_nodes = true;
    return query;
}

bool umi_workbench_layout_node_matches_query(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutNodeQuery *query)
{
    if (node == NULL || query == NULL) {
        return false;
    }
    if (umi_workbench_layout_text_present(query->node_id) &&
        !umi_workbench_layout_text_equal(
            node->node_id, query->node_id)) {
        return false;
    }
    if (umi_workbench_layout_text_present(query->component_id) &&
        !umi_workbench_layout_text_equal(
            node->component_id, query->component_id)) {
        return false;
    }
    if (umi_workbench_layout_text_present(
            query->owner_application_id) &&
        !umi_workbench_layout_text_equal(
            node->owner_application_id,
            query->owner_application_id)) {
        return false;
    }
    if (umi_workbench_layout_text_present(query->context_group_id) &&
        !umi_workbench_layout_text_equal(
            node->context_group_id, query->context_group_id)) {
        return false;
    }
    if (umi_workbench_layout_text_present(query->monitor_id) &&
        !umi_workbench_layout_text_equal(
            node->monitor_id, query->monitor_id)) {
        return false;
    }
    if (query->kind != 0 && node->kind != query->kind) {
        return false;
    }
    if (query->dock_region != 0 &&
        node->dock_region != query->dock_region) {
        return false;
    }
    if (query->visibility != 0 &&
        node->visibility != query->visibility) {
        return false;
    }
    if ((node->flags & query->required_flags) !=
        query->required_flags) {
        return false;
    }
    if ((node->flags & query->prohibited_flags) != 0U) {
        return false;
    }
    if (!query->include_containers &&
        umi_workbench_layout_node_is_container(node)) {
        return false;
    }
    if (!query->include_leaf_nodes &&
        umi_workbench_layout_node_is_leaf(node)) {
        return false;
    }
    return true;
}

UmiStatus umi_workbench_layout_query_nodes(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    UmiWorkbenchLayoutNodeQueryResult *out_result)
{
    size_t index;

    if (document == NULL || query == NULL || out_result == NULL ||
        query->structure_size < sizeof(*query)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    for (index = 0U; index < document->node_count; ++index) {
        if (!umi_workbench_layout_node_matches_query(
                &document->nodes[index], query)) {
            continue;
        }
        if (out_result->count < UMI_WORKBENCH_LAYOUT_MAX_NODES) {
            out_result->indices[out_result->count] = index;
        } else {
            out_result->truncated = true;
        }
        out_result->count += 1U;
    }

    if (out_result->count > UMI_WORKBENCH_LAYOUT_MAX_NODES) {
        out_result->count = UMI_WORKBENCH_LAYOUT_MAX_NODES;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_query_first(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    const UmiWorkbenchLayoutNode **out_node)
{
    size_t index;
    if (document == NULL || query == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_node = NULL;
    for (index = 0U; index < document->node_count; ++index) {
        if (umi_workbench_layout_node_matches_query(
                &document->nodes[index], query)) {
            *out_node = &document->nodes[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_workbench_layout_count_kind(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutNodeKind kind)
{
    size_t index;
    size_t count = 0U;
    if (document == NULL) {
        return 0U;
    }
    for (index = 0U; index < document->node_count; ++index) {
        if (document->nodes[index].kind == kind) {
            count += 1U;
        }
    }
    return count;
}

size_t umi_workbench_layout_count_owner(
    const UmiWorkbenchLayoutDocument *document,
    const char *owner_application_id)
{
    size_t index;
    size_t count = 0U;
    if (document == NULL || owner_application_id == NULL) {
        return 0U;
    }
    for (index = 0U; index < document->node_count; ++index) {
        if (umi_workbench_layout_text_equal(
                document->nodes[index].owner_application_id,
                owner_application_id)) {
            count += 1U;
        }
    }
    return count;
}

size_t umi_workbench_layout_count_component(
    const UmiWorkbenchLayoutDocument *document,
    const char *component_id)
{
    size_t index;
    size_t count = 0U;
    if (document == NULL || component_id == NULL) {
        return 0U;
    }
    for (index = 0U; index < document->node_count; ++index) {
        if (umi_workbench_layout_text_equal(
                document->nodes[index].component_id,
                component_id)) {
            count += 1U;
        }
    }
    return count;
}
