/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/view_container.c
 *
 * PURPOSE:
 *   Implement reusable workbench view-container metadata and active-view state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A view container can host several logical views while remaining independent of GTK list, stack or notebook widgets.
 */

#include "umicom/ui/view_container.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiViewContainerModel {
    UmiUiViewContainerSnapshot items[UMI_UI_VIEW_CONTAINER_MAX];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

static int compare_container(const void *left, const void *right)
{
    const UmiUiViewContainerSnapshot *a = (const UmiUiViewContainerSnapshot *)left;
    const UmiUiViewContainerSnapshot *b = (const UmiUiViewContainerSnapshot *)right;
    if (a->order < b->order) return -1;
    if (a->order > b->order) return 1;
    return strcmp(a->container_id, b->container_id);
}

static size_t find_item(const UmiUiViewContainerModel *model, const char *id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].container_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

static int contains_view(const UmiUiViewContainerSnapshot *item, const char *view_id)
{
    size_t index;
    for (index = 0U; index < item->view_count; ++index) {
        if (strcmp(item->view_ids[index], view_id) == 0) return 1;
    }
    return 0;
}

UmiStatus umi_ui_view_container_model_create(UmiUiViewContainerModel **out_model)
{
    UmiUiViewContainerModel *model;
    UmiStatus status;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiViewContainerModel *)calloc(1U, sizeof(*model));
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

void umi_ui_view_container_model_destroy(UmiUiViewContainerModel *model)
{
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

UmiStatus umi_ui_view_container_model_upsert(UmiUiViewContainerModel *model,
                                             const UmiUiViewContainerSnapshot *item)
{
    size_t index;
    if (model == NULL || item == NULL ||
        !umi_ui_id_is_valid(item->container_id) ||
        item->view_count > UMI_UI_VIEW_CONTAINER_MAX_VIEWS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item->container_id);
    if (index == SIZE_MAX) {
        if (model->count >= UMI_UI_VIEW_CONTAINER_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = model->count++;
    }
    if (item->active) {
        size_t active_index;
        for (active_index = 0U; active_index < model->count; ++active_index) {
            model->items[active_index].active = 0;
        }
    }
    model->items[index] = *item;
    qsort(model->items, model->count, sizeof(model->items[0]), compare_container);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_view_container_model_remove(UmiUiViewContainerModel *model,
                                             const char *container_id)
{
    size_t index;
    if (model == NULL || container_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, container_id);
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

UmiStatus umi_ui_view_container_model_find(const UmiUiViewContainerModel *model,
                                           const char *container_id,
                                           UmiUiViewContainerSnapshot *out_item)
{
    size_t index;
    if (model == NULL || container_id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, container_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_view_container_model_at(const UmiUiViewContainerModel *model,
                                         size_t index,
                                         UmiUiViewContainerSnapshot *out_item)
{
    if (model == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_view_container_model_set_active(UmiUiViewContainerModel *model,
                                                 const char *container_id)
{
    size_t index;
    size_t target;
    if (model == NULL || container_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    target = find_item(model, container_id);
    if (target == SIZE_MAX || !model->items[target].visible) {
        (void)umi_mutex_unlock(model->mutex);
        return target == SIZE_MAX ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < model->count; ++index) {
        model->items[index].active = index == target;
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_view_container_model_set_active_view(UmiUiViewContainerModel *model,
                                                      const char *container_id,
                                                      const char *view_id)
{
    size_t index;
    if (model == NULL || container_id == NULL || view_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, container_id);
    if (index == SIZE_MAX || !contains_view(&model->items[index], view_id)) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    (void)umi_ui_copy_text(model->items[index].active_view_id,
                           sizeof(model->items[index].active_view_id),
                           view_id);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_view_container_model_count(const UmiUiViewContainerModel *model)
{
    size_t count = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

uint64_t umi_ui_view_container_model_revision(const UmiUiViewContainerModel *model)
{
    uint64_t revision = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}
