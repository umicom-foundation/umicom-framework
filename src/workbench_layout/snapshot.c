/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/snapshot.c
 *
 * PURPOSE:
 *   Build immutable semantic layout snapshots for GTK4, Qt, Wt, web and headless consumers without exposing mutable persistence state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/snapshot.h"

#include <string.h>

#include "internal.h"

/* Provide the node is visible operation used by this module and its client applications. */
static bool node_is_visible(
    const UmiWorkbenchLayoutNode *node)
{
    return node != NULL &&
           node->visibility !=
               UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
}

/* Provide the node is panel operation used by this module and its client applications. */
static bool node_is_panel(
    const UmiWorkbenchLayoutNode *node)
{
    return node != NULL &&
           (node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
            node->kind ==
                UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP);
}

/* Provide the node is window operation used by this module and its client applications. */
static bool node_is_window(
    const UmiWorkbenchLayoutNode *node)
{
    return node != NULL &&
           (node->kind == UMI_WORKBENCH_LAYOUT_NODE_WINDOW ||
            node->kind ==
                UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW);
}

/*
 * Provide the populate node snapshot operation used by this module and its client
 * applications.
 */
static UmiStatus populate_node_snapshot(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutSession *session,
    size_t index,
    UmiWorkbenchLayoutNodeSnapshot *snapshot)
{
    const UmiWorkbenchLayoutNode *node;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || snapshot == NULL ||
        index >= document->node_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    node = &document->nodes[index];
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->structure_size = sizeof(*snapshot);

    status = umi_workbench_layout_copy_text(
        snapshot->node_id,
        sizeof(snapshot->node_id),
        node->node_id,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            snapshot->title,
            sizeof(snapshot->title),
            node->title,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            snapshot->component_id,
            sizeof(snapshot->component_id),
            node->component_id,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            snapshot->owner_application_id,
            sizeof(snapshot->owner_application_id),
            node->owner_application_id,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            snapshot->context_group_id,
            sizeof(snapshot->context_group_id),
            node->context_group_id,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            snapshot->monitor_id,
            sizeof(snapshot->monitor_id),
            node->monitor_id,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    snapshot->kind = node->kind;
    snapshot->orientation = node->orientation;
    snapshot->dock_region = node->dock_region;
    snapshot->visibility = node->visibility;
    snapshot->bounds = node->bounds;
    snapshot->split_ratio = node->split_ratio;
    snapshot->depth = umi_workbench_layout_node_depth(
        document, node->node_id);
    snapshot->child_count = node->child_count;
    snapshot->active =
        node->parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        document->nodes[node->parent_index].active_child_index ==
            umi_workbench_layout_node_child_position(
                &document->nodes[node->parent_index], index);
    snapshot->focused =
        session != NULL &&
        strcmp(session->active_node_id, node->node_id) == 0;
    snapshot->locked =
        umi_workbench_layout_node_has_flag(
            node, UMI_WORKBENCH_LAYOUT_NODE_LOCKED) ||
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout snapshot build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_snapshot_build(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutSession *session,
    UmiWorkbenchLayoutSnapshot *out_snapshot)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    out_snapshot->identity = document->identity;
    out_snapshot->version = document->version;
    (void)umi_workbench_layout_copy_text(
        out_snapshot->name,
        sizeof(out_snapshot->name),
        document->name,
        true);
    (void)umi_workbench_layout_copy_text(
        out_snapshot->category,
        sizeof(out_snapshot->category),
        document->category,
        true);
    out_snapshot->locked =
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED);
    out_snapshot->dirty =
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY);
    out_snapshot->content_hash = document->content_hash;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session != NULL) {
        (void)umi_workbench_layout_copy_text(
            out_snapshot->focused_node_id,
            sizeof(out_snapshot->focused_node_id),
            session->active_node_id,
            true);
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < document->node_count &&
         status == UMI_STATUS_OK;
         ++index) {
        UmiWorkbenchLayoutNodeSnapshot *node_snapshot =
            &out_snapshot->nodes[out_snapshot->node_count];
        const UmiWorkbenchLayoutNode *node =
            &document->nodes[index];

        status = populate_node_snapshot(
            document, session, index, node_snapshot);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            break;
        }

        out_snapshot->node_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (node_is_visible(node)) {
            out_snapshot->visible_node_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (node_is_panel(node)) {
            out_snapshot->panel_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (node_is_window(node)) {
            out_snapshot->window_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind ==
            UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) {
            out_snapshot->floating_window_count += 1U;
        }
    }
    return status;
}

/*
 * Find workbench layout snapshot while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchLayoutNodeSnapshot *
umi_workbench_layout_snapshot_find(
    const UmiWorkbenchLayoutSnapshot *snapshot,
    const char *node_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL ||
        !umi_workbench_layout_text_present(node_id)) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot->node_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                snapshot->nodes[index].node_id,
                node_id) == 0) {
            return &snapshot->nodes[index];
        }
    }
    return NULL;
}

/*
 * Find workbench layout snapshot while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchLayoutNodeSnapshot *
umi_workbench_layout_snapshot_at(
    const UmiWorkbenchLayoutSnapshot *snapshot,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL || index >= snapshot->node_count) {
        return NULL;
    }
    return &snapshot->nodes[index];
}
