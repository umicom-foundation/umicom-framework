/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/tree.c
 *
 * PURPOSE:
 *   Build and maintain a deterministic component tree projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/tree.h"
#include "internal.h"


void umi_workbench_designer_tree_init(UmiWorkbenchDesignerTree *tree)
{
    if (tree == NULL) return;
    (void)memset(tree, 0, sizeof(*tree));
}

bool umi_workbench_designer_tree_is_expanded(
    const UmiWorkbenchDesignerTree *tree,
    const char *node_id)
{
    size_t index;
    if (tree == NULL || node_id == NULL) return false;
    for (index = 0U; index < tree->collapsed_count; ++index) {
        if (strcmp(tree->collapsed_nodes[index].value, node_id) == 0) return false;
    }
    return true;
}

UmiStatus umi_workbench_designer_tree_set_expanded(
    UmiWorkbenchDesignerTree *tree,
    const char *node_id,
    bool expanded)
{
    size_t index;
    if (tree == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < tree->collapsed_count; ++index) {
        if (strcmp(tree->collapsed_nodes[index].value, node_id) == 0) {
            if (!expanded) return UMI_STATUS_OK;
            for (; index + 1U < tree->collapsed_count; ++index) {
                tree->collapsed_nodes[index] = tree->collapsed_nodes[index + 1U];
            }
            tree->collapsed_count -= 1U;
            tree->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (expanded) return UMI_STATUS_OK;
    if (tree->collapsed_count >= UMI_WORKBENCH_DESIGNER_MAX_TREE_ROWS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_workbench_designer_copy_text(
            tree->collapsed_nodes[tree->collapsed_count].value,
            sizeof(tree->collapsed_nodes[tree->collapsed_count].value),
            node_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    tree->collapsed_count += 1U;
    tree->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus tree_visit(
    UmiWorkbenchDesignerTree *tree,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    size_t node_index,
    size_t depth,
    bool *visited)
{
    const UmiWorkbenchLayoutNode *node;
    UmiWorkbenchDesignerTreeRow *row;
    const char *primary;
    size_t child_position;
    if (!umi_workbench_designer_document_index_valid(document, node_index) ||
        visited[node_index]) return UMI_STATUS_INVALID_STATE;
    if (tree->row_count >= UMI_WORKBENCH_DESIGNER_MAX_TREE_ROWS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    visited[node_index] = true;
    node = &document->nodes[node_index];
    row = &tree->rows[tree->row_count++];
    (void)memset(row, 0, sizeof(*row));
    (void)umi_workbench_designer_copy_text(row->node_id, sizeof(row->node_id), node->node_id);
    (void)umi_workbench_designer_copy_text(row->title, sizeof(row->title), node->title);
    (void)umi_workbench_designer_copy_text(row->component_id, sizeof(row->component_id), node->component_id);
    row->kind = node->kind;
    row->node_index = node_index;
    row->depth = depth;
    row->child_count = node->child_count;
    row->expanded = umi_workbench_designer_tree_is_expanded(tree, node->node_id);
    row->selected = selection != NULL &&
        umi_workbench_designer_selection_contains(selection, node->node_id);
    primary = selection != NULL ? umi_workbench_designer_selection_primary(selection) : NULL;
    row->primary = primary != NULL && strcmp(primary, node->node_id) == 0;
    row->visible = node->visibility != UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
    if (!row->expanded) return UMI_STATUS_OK;
    for (child_position = 0U; child_position < node->child_count; ++child_position) {
        UmiStatus status = tree_visit(
            tree, document, selection, node->child_indices[child_position],
            depth + 1U, visited);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_tree_build(
    UmiWorkbenchDesignerTree *tree,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection)
{
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    if (tree == NULL || document == NULL ||
        document->root_index >= document->node_count) return UMI_STATUS_INVALID_ARGUMENT;
    tree->row_count = 0U;
    (void)memset(visited, 0, sizeof(visited));
    tree->revision += 1U;
    return tree_visit(tree, document, selection, document->root_index, 0U, visited);
}

const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_tree_at(
    const UmiWorkbenchDesignerTree *tree,
    size_t index)
{
    if (tree == NULL || index >= tree->row_count) return NULL;
    return &tree->rows[index];
}

const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_tree_find(
    const UmiWorkbenchDesignerTree *tree,
    const char *node_id)
{
    size_t index;
    if (tree == NULL || node_id == NULL) return NULL;
    for (index = 0U; index < tree->row_count; ++index) {
        if (strcmp(tree->rows[index].node_id, node_id) == 0) return &tree->rows[index];
    }
    return NULL;
}
