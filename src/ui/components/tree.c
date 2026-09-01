/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/tree.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/tree.h"

#include <string.h>

void umi_ui_component_tree_init(UmiUiComponentTree *tree)
{
    if (tree != NULL) {
        (void)memset(tree, 0, sizeof(*tree));
    }
}

UmiStatus umi_ui_component_tree_add(UmiUiComponentTree *tree,
                                    const UmiUiComponentSpec *spec,
                                    int32_t parent_index,
                                    size_t *out_index)
{
    if (tree == NULL || spec == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (tree->count >= UMI_UI_COMPONENT_TREE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (parent_index >= 0 && (size_t)parent_index >= tree->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    const size_t index = tree->count;
    ++tree->count;
    tree->nodes[index].spec = *spec;
    tree->nodes[index].parent_index = parent_index;
    if (out_index != NULL) {
        *out_index = index;
    }
    return UMI_STATUS_OK;
}

size_t umi_ui_component_tree_child_count(const UmiUiComponentTree *tree,
                                         size_t parent_index)
{
    if (tree == NULL || parent_index >= tree->count) {
        return 0U;
    }
    size_t count = 0U;
    for (size_t index = 0U; index < tree->count; ++index) {
        if (tree->nodes[index].parent_index == (int32_t)parent_index) {
            ++count;
        }
    }
    return count;
}
