/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/split_model.c
 *
 * PURPOSE:
 *   Implement the split model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework terminal split-tree model.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/split_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiTerminalSplitModel {
    UmiTerminalSplitSnapshot nodes[UMI_TERMINAL_SPLIT_MAX];
    size_t count;
    uint64_t root_id;
    uint64_t next_id;
    uint64_t revision;
};

/* Provide the node index operation used by this module and its client applications. */
static size_t node_index(const UmiTerminalSplitModel *model, uint64_t node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || node_id == 0U) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->nodes[index].node_id == node_id) return index;
    }
    return SIZE_MAX;
}

/* Provide the remove index operation used by this module and its client applications. */
static void remove_index(UmiTerminalSplitModel *model, size_t index)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->count) {
        (void)memmove(&model->nodes[index], &model->nodes[index + 1U],
                      (model->count - index - 1U) * sizeof(model->nodes[0]));
    }
    model->count -= 1U;
}

/*
 * Initialise terminal split model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_terminal_split_model_create(const char *initial_session_id,
                                           UmiTerminalSplitModel **out_model)
{
    UmiTerminalSplitModel *model;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (initial_session_id == NULL || initial_session_id[0] == '\0' ||
        out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiTerminalSplitModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->next_id = 2U;
    model->root_id = 1U;
    model->count = 1U;
    model->nodes[0].node_id = 1U;
    model->nodes[0].leaf = 1;
    model->nodes[0].ratio = 0.5;
    (void)snprintf(model->nodes[0].session_id,
                   sizeof(model->nodes[0].session_id), "%s", initial_session_id);
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by terminal split model so the same storage can be reused
 * safely.
 */
void umi_terminal_split_model_destroy(UmiTerminalSplitModel *model) { free(model); }

/*
 * Provide the terminal split model split operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_split_model_split(UmiTerminalSplitModel *model,
                                         uint64_t leaf_id,
                                         const char *new_session_id,
                                         UmiTerminalOrientation orientation,
                                         double ratio,
                                         uint64_t *out_new_leaf_id)
{
    size_t index;
    UmiTerminalSplitSnapshot original;
    UmiTerminalSplitSnapshot *container;
    UmiTerminalSplitSnapshot *first;
    UmiTerminalSplitSnapshot *second;
    uint64_t first_id;
    uint64_t second_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || new_session_id == NULL || new_session_id[0] == '\0' ||
        ratio <= 0.05 || ratio >= 0.95) return UMI_STATUS_INVALID_ARGUMENT;
    index = node_index(model, leaf_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX || !model->nodes[index].leaf) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count + 2U > UMI_TERMINAL_SPLIT_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    original = model->nodes[index];
    first_id = model->next_id++;
    second_id = model->next_id++;
    container = &model->nodes[index];
    (void)memset(container, 0, sizeof(*container));
    container->node_id = leaf_id;
    container->parent_id = original.parent_id;
    container->first_child_id = first_id;
    container->second_child_id = second_id;
    container->orientation = orientation;
    container->ratio = ratio;
    first = &model->nodes[model->count++];
    *first = original;
    first->node_id = first_id;
    first->parent_id = leaf_id;
    first->first_child_id = 0U;
    first->second_child_id = 0U;
    second = &model->nodes[model->count++];
    (void)memset(second, 0, sizeof(*second));
    second->node_id = second_id;
    second->parent_id = leaf_id;
    second->leaf = 1;
    second->ratio = 0.5;
    (void)snprintf(second->session_id, sizeof(second->session_id),
                   "%s", new_session_id);
    model->revision += 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_new_leaf_id != NULL) *out_new_leaf_id = second_id;
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal split model close operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_split_model_close(UmiTerminalSplitModel *model,
                                         uint64_t leaf_id)
{
    size_t leaf_index;
    size_t parent_index;
    size_t sibling_index;
    UmiTerminalSplitSnapshot sibling;
    uint64_t sibling_id;
    uint64_t parent_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    leaf_index = node_index(model, leaf_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (leaf_index == SIZE_MAX || !model->nodes[leaf_index].leaf) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (model->nodes[leaf_index].parent_id == 0U) return UMI_STATUS_INVALID_STATE;
    parent_id = model->nodes[leaf_index].parent_id;
    parent_index = node_index(model, parent_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (parent_index == SIZE_MAX) return UMI_STATUS_INVALID_STATE;
    sibling_id = model->nodes[parent_index].first_child_id == leaf_id
        ? model->nodes[parent_index].second_child_id
        : model->nodes[parent_index].first_child_id;
    sibling_index = node_index(model, sibling_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (sibling_index == SIZE_MAX) return UMI_STATUS_INVALID_STATE;
    sibling = model->nodes[sibling_index];
    sibling.node_id = parent_id;
    sibling.parent_id = model->nodes[parent_index].parent_id;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!sibling.leaf) {
        size_t child;
        child = node_index(model, sibling.first_child_id);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (child != SIZE_MAX) model->nodes[child].parent_id = parent_id;
        child = node_index(model, sibling.second_child_id);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (child != SIZE_MAX) model->nodes[child].parent_id = parent_id;
    }
    model->nodes[parent_index] = sibling;
    leaf_index = node_index(model, leaf_id);
    sibling_index = node_index(model, sibling_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (leaf_index > sibling_index) {
        remove_index(model, leaf_index);
        sibling_index = node_index(model, sibling_id);
        remove_index(model, sibling_index);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        remove_index(model, sibling_index);
        leaf_index = node_index(model, leaf_id);
        remove_index(model, leaf_index);
    }
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal split model set ratio operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_split_model_set_ratio(UmiTerminalSplitModel *model,
                                             uint64_t node_id,
                                             double ratio)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || ratio <= 0.05 || ratio >= 0.95) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = node_index(model, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX || model->nodes[index].leaf) return UMI_STATUS_NOT_FOUND;
    model->nodes[index].ratio = ratio;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find terminal split model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_split_model_at(const UmiTerminalSplitModel *model,
                                      size_t index,
                                      UmiTerminalSplitSnapshot *out_node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_node = model->nodes[index];
    return UMI_STATUS_OK;
}

/*
 * Find terminal split model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_split_model_find(const UmiTerminalSplitModel *model,
                                        uint64_t node_id,
                                        UmiTerminalSplitSnapshot *out_node)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = node_index(model, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_node = model->nodes[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by terminal split model without changing their
 * state.
 */
size_t umi_terminal_split_model_count(const UmiTerminalSplitModel *model)
{ return model != NULL ? model->count : 0U; }
/*
 * Provide the terminal split model root id operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_split_model_root_id(const UmiTerminalSplitModel *model)
{ return model != NULL ? model->root_id : 0U; }
/*
 * Provide the terminal split model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_split_model_revision(const UmiTerminalSplitModel *model)
{ return model != NULL ? model->revision : 0U; }
