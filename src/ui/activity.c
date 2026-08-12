/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/activity.c
 *
 * PURPOSE:
 *   Implement the Activity Bar model with deterministic ordering-neutral storage and one active item.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The model stores metadata only. Frontend adapters decide whether an activity is shown as an icon, button, tab or menu item.
 */

#include "umicom/ui/activity.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiActivityModel {
    UmiUiActivitySnapshot items[UMI_UI_ACTIVITY_MAX];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

static int compare_activity(const void *left, const void *right)
{
    const UmiUiActivitySnapshot *a = (const UmiUiActivitySnapshot *)left;
    const UmiUiActivitySnapshot *b = (const UmiUiActivitySnapshot *)right;
    if (a->order < b->order) return -1;
    if (a->order > b->order) return 1;
    return strcmp(a->activity_id, b->activity_id);
}

static size_t find_item(const UmiUiActivityModel *model, const char *id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].activity_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_activity_model_create(UmiUiActivityModel **out_model)
{
    UmiUiActivityModel *model;
    UmiStatus status;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiActivityModel *)calloc(1U, sizeof(*model));
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

void umi_ui_activity_model_destroy(UmiUiActivityModel *model)
{
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

UmiStatus umi_ui_activity_model_upsert(UmiUiActivityModel *model,
                                       const UmiUiActivitySnapshot *item)
{
    size_t index;
    if (model == NULL || item == NULL ||
        !umi_ui_id_is_valid(item->activity_id) ||
        !umi_ui_id_is_valid(item->container_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item->activity_id);
    if (index == SIZE_MAX) {
        if (model->count >= UMI_UI_ACTIVITY_MAX) {
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
    qsort(model->items, model->count, sizeof(model->items[0]), compare_activity);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_activity_model_remove(UmiUiActivityModel *model,
                                       const char *activity_id)
{
    size_t index;
    if (model == NULL || activity_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, activity_id);
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

UmiStatus umi_ui_activity_model_find(const UmiUiActivityModel *model,
                                     const char *activity_id,
                                     UmiUiActivitySnapshot *out_item)
{
    size_t index;
    if (model == NULL || activity_id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, activity_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_activity_model_at(const UmiUiActivityModel *model,
                                   size_t index,
                                   UmiUiActivitySnapshot *out_item)
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

UmiStatus umi_ui_activity_model_set_active(UmiUiActivityModel *model,
                                          const char *activity_id)
{
    size_t index;
    size_t target;
    if (model == NULL || activity_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    target = find_item(model, activity_id);
    if (target == SIZE_MAX || !model->items[target].enabled ||
        !model->items[target].visible) {
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

size_t umi_ui_activity_model_count(const UmiUiActivityModel *model)
{
    size_t count = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

uint64_t umi_ui_activity_model_revision(const UmiUiActivityModel *model)
{
    uint64_t revision = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}
