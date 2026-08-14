/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/component_tree_v2.c
 * PURPOSE: Build hierarchy snapshots and perform undoable reparenting.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/component_tree_v2.h"

#include <stdio.h>
#include <string.h>

static const UmiDesignerTreeNodeV2 *find_node(
    const UmiDesignerComponentTreeV2 *tree, const char *id)
{
    size_t index;
    if (tree == NULL || id == NULL) return NULL;
    for (index = 0U; index < tree->count; ++index) {
        if (strcmp(tree->nodes[index].node_id, id) == 0) return &tree->nodes[index];
    }
    return NULL;
}

static uint32_t depth_of(const UmiDesignerComponentTreeV2 *tree,
                         const UmiDesignerTreeNodeV2 *node)
{
    uint32_t depth = 0U;
    const UmiDesignerTreeNodeV2 *current = node;
    while (current != NULL && current->parent_id[0] != '\0' &&
           strcmp(current->parent_id, "-") != 0 &&
           depth < UMI_DESIGNER_V2_MAX_TREE_NODES) {
        current = find_node(tree, current->parent_id);
        if (current != NULL) depth++;
    }
    return depth;
}

UmiStatus umi_designer_component_tree_v2_build(
    const UmiDesignerDocument *document,
    const UmiDesignerSelection *selection,
    UmiDesignerComponentTreeV2 *out_tree)
{
    UmiDesignerDocumentSnapshot snapshot;
    UmiDeclDocument *declarative;
    size_t index;
    UmiStatus status;
    if (document == NULL || out_tree == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_tree, 0, sizeof(*out_tree));
    status = umi_designer_document_snapshot(document, &snapshot);
    declarative = umi_designer_document_declarative((UmiDesignerDocument *)document);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.component_count > UMI_DESIGNER_V2_MAX_TREE_NODES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < snapshot.component_count; ++index) {
        UmiDeclNode node;
        UmiDesignerTreeNodeV2 *tree_node = &out_tree->nodes[index];
        status = umi_decl_document_node_at(declarative, index, &node);
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
    for (index = 0U; index < out_tree->count; ++index) {
        size_t other;
        uint32_t sibling = 0U;
        UmiDesignerTreeNodeV2 *node = &out_tree->nodes[index];
        node->depth = depth_of(out_tree, node);
        for (other = 0U; other < out_tree->count; ++other) {
            if (strcmp(out_tree->nodes[other].parent_id, node->node_id) == 0) {
                node->child_count++;
            }
            if (other < index && strcmp(out_tree->nodes[other].parent_id,
                                        node->parent_id) == 0) sibling++;
        }
        node->sibling_index = sibling;
    }
    out_tree->source_revision = snapshot.revision;
    return UMI_STATUS_OK;
}

const UmiDesignerTreeNodeV2 *umi_designer_component_tree_v2_find(
    const UmiDesignerComponentTreeV2 *tree,
    const char *node_id)
{
    return find_node(tree, node_id);
}

static int would_create_cycle(UmiDeclDocument *document,
                              const char *node_id,
                              const char *new_parent_id)
{
    char current_id[UMI_DECL_ID_CAPACITY];
    size_t remaining = umi_decl_document_node_count(document);
    if (umi_decl_copy_text(current_id, sizeof(current_id), new_parent_id) != UMI_STATUS_OK) return 1;
    while (remaining-- > 0U && current_id[0] != '\0' && strcmp(current_id, "-") != 0) {
        UmiDeclNode current;
        if (strcmp(current_id, node_id) == 0) return 1;
        if (umi_decl_document_find_node(document, current_id, &current) != UMI_STATUS_OK) return 0;
        (void)umi_decl_copy_text(current_id, sizeof(current_id), current.parent_id);
    }
    return 0;
}

UmiStatus umi_designer_component_tree_v2_reparent(
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
    if (history == NULL || document == NULL || node_id == NULL ||
        new_parent_id == NULL || strcmp(node_id, new_parent_id) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    declarative = umi_designer_document_declarative(document);
    status = umi_decl_document_find_node(declarative, node_id, &before);
    if (status == UMI_STATUS_OK) status = umi_decl_document_find_node(
        declarative, new_parent_id, &parent);
    if (status != UMI_STATUS_OK) return status;
    if (would_create_cycle(declarative, node_id, new_parent_id)) return UMI_STATUS_INVALID_STATE;
    after = before;
    (void)umi_decl_copy_text(after.parent_id, sizeof(after.parent_id), new_parent_id);
    (void)snprintf(order_text, sizeof(order_text), "%u", sibling_index);
    status = umi_decl_node_set_attribute(
        &after, "designer.order", UMI_DECL_VALUE_INTEGER, order_text);
    if (status == UMI_STATUS_OK) status = umi_designer_operation_move(
        &before, &after, &operation);
    if (status == UMI_STATUS_OK) status = umi_designer_history_execute(
        history, &operation);
    return status;
}
