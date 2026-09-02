/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/tab_editor.c
 *
 * PURPOSE:
 *   Implement tab-group mutations and active-child invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/tab_editor.h"
#include "internal.h"
#include <stdio.h>


/* Provide the tab make id operation used by this module and its client applications. */
static UmiStatus tab_make_id(
    const UmiWorkbenchLayoutDocument *document,
    const char *target_node_id,
    char *out_id,
    size_t capacity)
{
    size_t attempt;
    /* Visit each bounded item once so every record receives the same rule. */
    for (attempt = 0U; attempt < 1000U; ++attempt) {
        int written = snprintf(out_id, capacity, "%s-tabs-%zu", target_node_id, attempt);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (umi_workbench_layout_document_find_node(document, out_id) == NULL) return UMI_STATUS_OK;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Provide the workbench designer tab wrap node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_tab_wrap_node(
    UmiWorkbenchLayoutDocument *document,
    const char *target_node_id,
    const UmiWorkbenchLayoutNode *new_node,
    bool new_node_after,
    char *out_tab_group_id,
    size_t out_capacity)
{
    size_t target_index;
    size_t parent_index;
    size_t parent_position = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    size_t new_index;
    size_t group_index;
    char group_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchLayoutNode group;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || target_node_id == NULL || new_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    target_index = umi_workbench_layout_document_find_node_index(document, target_node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (target_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    parent_index = umi_workbench_designer_parent_index(document, target_index);
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        parent_position = umi_workbench_layout_node_child_position(
            &document->nodes[parent_index], target_index);
    }
    status = tab_make_id(document, target_node_id, group_id, sizeof(group_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_workbench_layout_node_init(
        &group, group_id, UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP);
    status = umi_workbench_layout_node_set_title(&group, "Tab Group");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_layout_document_add_node(document, new_node, &new_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_layout_document_add_node(document, &group, &group_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    document->nodes[group_index].child_count = 2U;
    document->nodes[group_index].child_indices[new_node_after ? 0U : 1U] = target_index;
    document->nodes[group_index].child_indices[new_node_after ? 1U : 0U] = new_index;
    document->nodes[group_index].active_child_index = new_node_after ? 1U : 0U;
    document->nodes[target_index].parent_index = group_index;
    document->nodes[new_index].parent_index = group_index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        document->root_index = group_index;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (parent_position == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_INVALID_STATE;
        document->nodes[parent_index].child_indices[parent_position] = group_index;
        document->nodes[group_index].parent_index = parent_index;
    }
    umi_workbench_layout_document_increment_revision(document);
    umi_workbench_layout_document_refresh_hash(document);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_tab_group_id != NULL && out_capacity > 0U) {
        return umi_workbench_designer_copy_text(out_tab_group_id, out_capacity, group_id);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer tab reorder operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_tab_reorder(
    UmiWorkbenchLayoutDocument *document,
    const char *tab_group_id,
    const char *child_node_id,
    size_t new_position)
{
    UmiWorkbenchLayoutNode *group;
    size_t child_index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || tab_group_id == NULL || child_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group = umi_workbench_layout_document_find_node_mutable(document, tab_group_id);
    child_index = umi_workbench_layout_document_find_node_index(document, child_node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group == NULL || child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (group->kind != UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP) return UMI_STATUS_INVALID_STATE;
    status = umi_workbench_layout_node_move_child_index(group, child_index, new_position);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    return status;
}

/*
 * Provide the workbench designer tab activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_tab_activate(
    UmiWorkbenchLayoutDocument *document,
    const char *tab_group_id,
    const char *child_node_id)
{
    UmiWorkbenchLayoutNode *group;
    size_t child_index;
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || tab_group_id == NULL || child_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group = umi_workbench_layout_document_find_node_mutable(document, tab_group_id);
    child_index = umi_workbench_layout_document_find_node_index(document, child_node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group == NULL || child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (group->kind != UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP) return UMI_STATUS_INVALID_STATE;
    position = umi_workbench_layout_node_child_position(group, child_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    group->active_child_index = position;
    group->revision += 1U;
    umi_workbench_layout_document_increment_revision(document);
    umi_workbench_layout_document_refresh_hash(document);
    return UMI_STATUS_OK;
}
