/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/split_editor.c
 *
 * PURPOSE:
 *   Implement split creation without requiring a frontend to edit raw child
 *   indices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/split_editor.h"
#include "internal.h"
#include <stdio.h>


/* Provide the split make id operation used by this module and its client applications. */
static UmiStatus split_make_id(
    const UmiWorkbenchLayoutDocument *document,
    const char *target_node_id,
    char *out_id,
    size_t capacity)
{
    size_t attempt;
    /* Visit each bounded item once so every record receives the same rule. */
    for (attempt = 0U; attempt < 1000U; ++attempt) {
        int written = snprintf(out_id, capacity, "%s-split-%zu", target_node_id, attempt);
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
 * Provide the workbench designer split wrap node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_split_wrap_node(
    UmiWorkbenchLayoutDocument *document,
    const char *target_node_id,
    const UmiWorkbenchLayoutNode *new_node,
    UmiWorkbenchLayoutOrientation orientation,
    double ratio,
    bool new_node_after,
    char *out_split_node_id,
    size_t out_capacity)
{
    size_t target_index;
    size_t parent_index;
    size_t new_index;
    size_t split_index;
    size_t parent_position = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    char split_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchLayoutNode split;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || target_node_id == NULL || new_node == NULL ||
        orientation == UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE ||
        ratio <= 0.0 || ratio >= 1.0) return UMI_STATUS_INVALID_ARGUMENT;
    target_index = umi_workbench_layout_document_find_node_index(document, target_node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (target_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_workbench_layout_document_find_node(document, new_node->node_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    parent_index = umi_workbench_designer_parent_index(document, target_index);
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        parent_position = umi_workbench_layout_node_child_position(
            &document->nodes[parent_index], target_index);
    }
    status = split_make_id(document, target_node_id, split_id, sizeof(split_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_workbench_layout_node_init(&split, split_id, UMI_WORKBENCH_LAYOUT_NODE_SPLIT);
    status = umi_workbench_layout_node_set_title(&split, "Split Region");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_layout_node_set_split(&split, orientation, ratio);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_layout_document_add_node(document, new_node, &new_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_layout_document_add_node(document, &split, &split_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    split = document->nodes[split_index];
    split.child_count = 2U;
    split.child_indices[new_node_after ? 0U : 1U] = target_index;
    split.child_indices[new_node_after ? 1U : 0U] = new_index;
    split.active_child_index = 0U;
    document->nodes[split_index] = split;
    document->nodes[target_index].parent_index = split_index;
    document->nodes[new_index].parent_index = split_index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        document->root_index = split_index;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (parent_position == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_INVALID_STATE;
        document->nodes[parent_index].child_indices[parent_position] = split_index;
        document->nodes[split_index].parent_index = parent_index;
    }
    umi_workbench_layout_document_increment_revision(document);
    umi_workbench_layout_document_refresh_hash(document);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_split_node_id != NULL && out_capacity > 0U) {
        return umi_workbench_designer_copy_text(out_split_node_id, out_capacity, split_id);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer split set ratio operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_split_set_ratio(
    UmiWorkbenchLayoutDocument *document,
    const char *split_node_id,
    double ratio)
{
    UmiWorkbenchLayoutNode *node;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || split_node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    node = umi_workbench_layout_document_find_node_mutable(document, split_node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (node->kind != UMI_WORKBENCH_LAYOUT_NODE_SPLIT) return UMI_STATUS_INVALID_STATE;
    status = umi_workbench_layout_node_set_split(node, node->orientation, ratio);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
    }
    return status;
}
