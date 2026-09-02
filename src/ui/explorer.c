/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/explorer.c
 *
 * PURPOSE:
 *   Implement bounded Explorer tree state, selection, expansion and fuzzy search.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The model is intentionally storage-neutral. It can represent filesystem paths, remote resources or virtual product trees.
 */

#include "umicom/ui/explorer.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"
#include "workbench_service_internal.h"

struct UmiUiExplorerModel {
    UmiUiExplorerNode items[UMI_UI_EXPLORER_MAX_NODES];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiExplorerModel *model, const char *id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].node_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the insert result operation used by this module and its client applications. */
static void insert_result(UmiUiExplorerSearchResults *results,
                          size_t node_index,
                          int score)
{
    size_t position = results->count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (score < 0) return;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position < UMI_UI_EXPLORER_MAX_RESULTS) {
        ++results->count;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (results->items[position - 1U].score >= score) {
        return;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        position = UMI_UI_EXPLORER_MAX_RESULTS - 1U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= results->count) position = results->count - 1U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (position > 0U && results->items[position - 1U].score < score) {
        results->items[position] = results->items[position - 1U];
        --position;
    }
    results->items[position].node_index = node_index;
    results->items[position].score = score;
}

/*
 * Initialise ui explorer model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_explorer_model_create(UmiUiExplorerModel **out_model)
{
    UmiUiExplorerModel *model;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiExplorerModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&model->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(model);
        return status;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ui explorer model so the same storage can be reused
 * safely.
 */
void umi_ui_explorer_model_destroy(UmiUiExplorerModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

/*
 * Provide the ui explorer model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_model_upsert(UmiUiExplorerModel *model,
                                       const UmiUiExplorerNode *node)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || node == NULL ||
        !umi_ui_id_is_valid(node->node_id) ||
        node->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node->node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count >= UMI_UI_EXPLORER_MAX_NODES) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = model->count++;
    }
    model->items[index] = *node;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Remove ui explorer model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_explorer_model_remove(UmiUiExplorerModel *model,
                                       const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index],
                      &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(model->items[0]));
    }
    --model->count;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find ui explorer model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_explorer_model_find(const UmiUiExplorerModel *model,
                                     const char *node_id,
                                     UmiUiExplorerNode *out_node)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || node_id == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_node = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find ui explorer model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_explorer_model_at(const UmiUiExplorerModel *model,
                                   size_t index,
                                   UmiUiExplorerNode *out_node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_node = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui explorer model select operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_model_select(UmiUiExplorerModel *model,
                                       const char *node_id)
{
    size_t index;
    size_t target;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    target = find_item(model, node_id);
    /* Configure the optional target only when its feature has created it. */
    if (target == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        model->items[index].selected = index == target;
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui explorer model set expanded operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_model_set_expanded(UmiUiExplorerModel *model,
                                             const char *node_id,
                                             int expanded)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].expanded = expanded != 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui explorer search operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_search(const UmiUiExplorerModel *model,
                                 const char *query,
                                 UmiUiExplorerSearchResults *out_results)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || query == NULL || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_results, 0, sizeof(*out_results));
    (void)umi_mutex_lock(model->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        int score = umi_ui_fuzzy_score_ci(query, model->items[index].name);
        int path_score = umi_ui_fuzzy_score_ci(query, model->items[index].path);
        /* Apply this branch only when its contract condition is satisfied. */
        if (path_score > score) score = path_score;
        insert_result(out_results, index, score);
    }
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ui explorer model without changing their
 * state.
 */
size_t umi_ui_explorer_model_count(const UmiUiExplorerModel *model)
{
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

/*
 * Provide the ui explorer model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_explorer_model_revision(const UmiUiExplorerModel *model)
{
    uint64_t revision = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}
