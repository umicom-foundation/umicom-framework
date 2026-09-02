/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/query.c
 *
 * PURPOSE:
 *   Implement bounded semantic node queries for layout browsers, designers, workbench composition and architecture validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/query.h"

#include <string.h>

#include "internal.h"

/*
 * Provide the workbench layout node query default operation used by this module and its
 * client applications.
 */
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

/*
 * Provide the workbench layout node matches query operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_node_matches_query(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutNodeQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL || query == NULL) {
        return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(query->node_id) &&
        !umi_workbench_layout_text_equal(
            node->node_id, query->node_id)) {
        return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(query->component_id) &&
        !umi_workbench_layout_text_equal(
            node->component_id, query->component_id)) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_text_present(
            query->owner_application_id) &&
        !umi_workbench_layout_text_equal(
            node->owner_application_id,
            query->owner_application_id)) {
        return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(query->context_group_id) &&
        !umi_workbench_layout_text_equal(
            node->context_group_id, query->context_group_id)) {
        return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(query->monitor_id) &&
        !umi_workbench_layout_text_equal(
            node->monitor_id, query->monitor_id)) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->kind != 0 && node->kind != query->kind) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->dock_region != 0 &&
        node->dock_region != query->dock_region) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->visibility != 0 &&
        node->visibility != query->visibility) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((node->flags & query->required_flags) !=
        query->required_flags) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((node->flags & query->prohibited_flags) != 0U) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!query->include_containers &&
        umi_workbench_layout_node_is_container(node)) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!query->include_leaf_nodes &&
        umi_workbench_layout_node_is_leaf(node)) {
        return false;
    }
    return true;
}

/*
 * Provide the workbench layout query nodes operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_query_nodes(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    UmiWorkbenchLayoutNodeQueryResult *out_result)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || query == NULL || out_result == NULL ||
        query->structure_size < sizeof(*query)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_layout_node_matches_query(
                &document->nodes[index], query)) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_result->count < UMI_WORKBENCH_LAYOUT_MAX_NODES) {
            out_result->indices[out_result->count] = index;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_result->truncated = true;
        }
        out_result->count += 1U;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (out_result->count > UMI_WORKBENCH_LAYOUT_MAX_NODES) {
        out_result->count = UMI_WORKBENCH_LAYOUT_MAX_NODES;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout query first operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_query_first(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNodeQuery *query,
    const UmiWorkbenchLayoutNode **out_node)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || query == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_node = NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_workbench_layout_node_matches_query(
                &document->nodes[index], query)) {
            *out_node = &document->nodes[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench layout count kind operation used by this module and its client
 * applications.
 */
size_t umi_workbench_layout_count_kind(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutNodeKind kind)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (document->nodes[index].kind == kind) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Provide the workbench layout count owner operation used by this module and its client
 * applications.
 */
size_t umi_workbench_layout_count_owner(
    const UmiWorkbenchLayoutDocument *document,
    const char *owner_application_id)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || owner_application_id == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_workbench_layout_text_equal(
                document->nodes[index].owner_application_id,
                owner_application_id)) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Provide the workbench layout count component operation used by this module and its
 * client applications.
 */
size_t umi_workbench_layout_count_component(
    const UmiWorkbenchLayoutDocument *document,
    const char *component_id)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || component_id == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_workbench_layout_text_equal(
                document->nodes[index].component_id,
                component_id)) {
            count += 1U;
        }
    }
    return count;
}
