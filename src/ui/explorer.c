/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/explorer.c
 *
 * PURPOSE:
 *   Implement bounded Explorer tree state, selection, expansion and fuzzy search.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

static size_t find_item(const UmiUiExplorerModel *model, const char *id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].node_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

static void insert_result(UmiUiExplorerSearchResults *results,
                          size_t node_index,
                          int score)
{
    size_t position = results->count;
    if (score < 0) return;
    if (position < UMI_UI_EXPLORER_MAX_RESULTS) {
        ++results->count;
    } else if (results->items[position - 1U].score >= score) {
        return;
    } else {
        position = UMI_UI_EXPLORER_MAX_RESULTS - 1U;
    }
    if (position >= results->count) position = results->count - 1U;
    while (position > 0U && results->items[position - 1U].score < score) {
        results->items[position] = results->items[position - 1U];
        --position;
    }
    results->items[position].node_index = node_index;
    results->items[position].score = score;
}

UmiStatus umi_ui_explorer_model_create(UmiUiExplorerModel **out_model)
{
    UmiUiExplorerModel *model;
    UmiStatus status;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiExplorerModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&model->mutex);
    if (status != UMI_STATUS_OK) {
        free(model);
        return status;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_ui_explorer_model_destroy(UmiUiExplorerModel *model)
{
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

UmiStatus umi_ui_explorer_model_upsert(UmiUiExplorerModel *model,
                                       const UmiUiExplorerNode *node)
{
    size_t index;
    if (model == NULL || node == NULL ||
        !umi_ui_id_is_valid(node->node_id) ||
        node->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node->node_id);
    if (index == SIZE_MAX) {
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

UmiStatus umi_ui_explorer_model_remove(UmiUiExplorerModel *model,
                                       const char *node_id)
{
    size_t index;
    if (model == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
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

UmiStatus umi_ui_explorer_model_find(const UmiUiExplorerModel *model,
                                     const char *node_id,
                                     UmiUiExplorerNode *out_node)
{
    size_t index;
    if (model == NULL || node_id == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_node = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_explorer_model_at(const UmiUiExplorerModel *model,
                                   size_t index,
                                   UmiUiExplorerNode *out_node)
{
    if (model == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_node = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_explorer_model_select(UmiUiExplorerModel *model,
                                       const char *node_id)
{
    size_t index;
    size_t target;
    if (model == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    target = find_item(model, node_id);
    if (target == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (index = 0U; index < model->count; ++index) {
        model->items[index].selected = index == target;
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_explorer_model_set_expanded(UmiUiExplorerModel *model,
                                             const char *node_id,
                                             int expanded)
{
    size_t index;
    if (model == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, node_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].expanded = expanded != 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_explorer_search(const UmiUiExplorerModel *model,
                                 const char *query,
                                 UmiUiExplorerSearchResults *out_results)
{
    size_t index;
    if (model == NULL || query == NULL || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_results, 0, sizeof(*out_results));
    (void)umi_mutex_lock(model->mutex);
    for (index = 0U; index < model->count; ++index) {
        int score = umi_ui_fuzzy_score_ci(query, model->items[index].name);
        int path_score = umi_ui_fuzzy_score_ci(query, model->items[index].path);
        if (path_score > score) score = path_score;
        insert_result(out_results, index, score);
    }
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_explorer_model_count(const UmiUiExplorerModel *model)
{
    size_t count = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

uint64_t umi_ui_explorer_model_revision(const UmiUiExplorerModel *model)
{
    uint64_t revision = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}
