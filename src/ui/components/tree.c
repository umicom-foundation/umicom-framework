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

/*
 * Initialise ui component tree from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_component_tree_init(UmiUiComponentTree *tree)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree != NULL) {
        (void)memset(tree, 0, sizeof(*tree));
    }
}

/* Add ui component tree only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_component_tree_add(UmiUiComponentTree *tree,
                                    const UmiUiComponentSpec *spec,
                                    int32_t parent_index,
                                    size_t *out_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || spec == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tree->count >= UMI_UI_COMPONENT_TREE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (parent_index >= 0 && (size_t)parent_index >= tree->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    const size_t index = tree->count;
    ++tree->count;
    tree->nodes[index].spec = *spec;
    tree->nodes[index].parent_index = parent_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_index != NULL) {
        *out_index = index;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ui component tree child without changing
 * their state.
 */
size_t umi_ui_component_tree_child_count(const UmiUiComponentTree *tree,
                                         size_t parent_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || parent_index >= tree->count) {
        return 0U;
    }
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t index = 0U; index < tree->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (tree->nodes[index].parent_index == (int32_t)parent_index) {
            ++count;
        }
    }
    return count;
}
