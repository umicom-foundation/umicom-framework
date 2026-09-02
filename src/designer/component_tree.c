/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/component_tree.c
 * PURPOSE: Build hierarchy snapshots and perform undoable reparenting.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/component_tree.h"

#include <stdio.h>
#include <string.h>

/* Provide the find node operation used by this module and its client applications. */
static const UmiDesignerTreeNode *find_node(
    const UmiDesignerComponentTree *tree, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tree->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(tree->nodes[index].node_id, id) == 0) return &tree->nodes[index];
    }
    return NULL;
}

/* Provide the depth of operation used by this module and its client applications. */
static uint32_t depth_of(const UmiDesignerComponentTree *tree,
                         const UmiDesignerTreeNode *node)
{
    uint32_t depth = 0U;
    const UmiDesignerTreeNode *current = node;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (current != NULL && current->parent_id[0] != '\0' &&
           strcmp(current->parent_id, "-") != 0 &&
           depth < UMI_DESIGNER_MAX_TREE_NODES) {
        current = find_node(tree, current->parent_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (current != NULL) depth++;
    }
    return depth;
}

/*
 * Provide the designer component tree build operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_component_tree_build(
    const UmiDesignerDocument *document,
    const UmiDesignerSelection *selection,
    UmiDesignerComponentTree *out_tree)
{
    UmiDesignerDocumentSnapshot snapshot;
    UmiDeclDocument *declarative;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_tree == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_tree, 0, sizeof(*out_tree));
    status = umi_designer_document_snapshot(document, &snapshot);
    declarative = umi_designer_document_declarative((UmiDesignerDocument *)document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.component_count > UMI_DESIGNER_MAX_TREE_NODES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.component_count; ++index) {
        UmiDeclNode node;
        UmiDesignerTreeNode *tree_node = &out_tree->nodes[index];
        status = umi_decl_document_node_at(declarative, index, &node);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)umi_decl_copy_text(tree_node->node_id, sizeof(tree_node->node_id),
                                 node.node_id);
        (void)umi_decl_copy_text(tree_node->component_type,
                                 sizeof(tree_node->component_type),
                                 node.component_type);
        (void)umi_decl_copy_text(tree_node->parent_id,
                                 sizeof(tree_node->parent_id), node.parent_id);
        tree_node->selected = selection != NULL &&
                              umi_designer_selection_contains(selection, node.node_id);
        out_tree->count++;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_tree->count; ++index) {
        size_t other;
        uint32_t sibling = 0U;
        UmiDesignerTreeNode *node = &out_tree->nodes[index];
        node->depth = depth_of(out_tree, node);
        /* Visit each bounded item once so every record receives the same rule. */
        for (other = 0U; other < out_tree->count; ++other) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(out_tree->nodes[other].parent_id, node->node_id) == 0) {
                node->child_count++;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (other < index && strcmp(out_tree->nodes[other].parent_id,
                                        node->parent_id) == 0) sibling++;
        }
        node->sibling_index = sibling;
    }
    out_tree->source_revision = snapshot.revision;
    return UMI_STATUS_OK;
}

/*
 * Find designer component tree while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesignerTreeNode *umi_designer_component_tree_find(
    const UmiDesignerComponentTree *tree,
    const char *node_id)
{
    return find_node(tree, node_id);
}

/*
 * Provide the would create cycle operation used by this module and its client
 * applications.
 */
static int would_create_cycle(UmiDeclDocument *document,
                              const char *node_id,
                              const char *new_parent_id)
{
    char current_id[UMI_DECL_ID_CAPACITY];
    size_t remaining = umi_decl_document_node_count(document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_decl_copy_text(current_id, sizeof(current_id), new_parent_id) != UMI_STATUS_OK) return 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (remaining-- > 0U && current_id[0] != '\0' && strcmp(current_id, "-") != 0) {
        UmiDeclNode current;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(current_id, node_id) == 0) return 1;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_decl_document_find_node(document, current_id, &current) != UMI_STATUS_OK) return 0;
        (void)umi_decl_copy_text(current_id, sizeof(current_id), current.parent_id);
    }
    return 0;
}

/*
 * Provide the designer component tree reparent operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_component_tree_reparent(
    UmiDesignerHistory *history,
    UmiDesignerDocument *document,
    const char *node_id,
    const char *new_parent_id,
    uint32_t sibling_index)
{
    UmiDeclDocument *declarative;
    UmiDeclNode before;
    UmiDeclNode after;
    UmiDeclNode parent;
    UmiDesignerOperation operation;
    char order_text[32];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || document == NULL || node_id == NULL ||
        new_parent_id == NULL || strcmp(node_id, new_parent_id) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    declarative = umi_designer_document_declarative(document);
    status = umi_decl_document_find_node(declarative, node_id, &before);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_document_find_node(
        declarative, new_parent_id, &parent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (would_create_cycle(declarative, node_id, new_parent_id)) return UMI_STATUS_INVALID_STATE;
    after = before;
    (void)umi_decl_copy_text(after.parent_id, sizeof(after.parent_id), new_parent_id);
    (void)snprintf(order_text, sizeof(order_text), "%u", sibling_index);
    status = umi_decl_node_set_attribute(
        &after, "designer.order", UMI_DECL_VALUE_INTEGER, order_text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_operation_move(
        &before, &after, &operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_history_execute(
        history, &operation);
    return status;
}
