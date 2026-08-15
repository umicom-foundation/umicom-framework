/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/document_view.c
 *
 * PURPOSE:
 *   Implement the toolkit-neutral document-tab model including preview,
 *   grouping, navigation, activation, close safety and pinning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/document_view.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiDocumentViewModel {
    UmiUiDocumentViewSnapshot items[UMI_UI_DOCUMENT_VIEW_MAX];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

static size_t find_item(const UmiUiDocumentViewModel *model, const char *id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].view_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

static const char *effective_group(const UmiUiDocumentViewSnapshot *item)
{
    return item->group_id[0] != '\0'
        ? item->group_id
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

static int same_group(const UmiUiDocumentViewSnapshot *left,
                      const UmiUiDocumentViewSnapshot *right)
{
    return strcmp(effective_group(left), effective_group(right)) == 0;
}

static void erase_at(UmiUiDocumentViewModel *model, size_t index)
{
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index],
                      &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(model->items[0]));
    }
    model->count -= 1U;
}

static void insert_at(UmiUiDocumentViewModel *model,
                      size_t index,
                      const UmiUiDocumentViewSnapshot *item)
{
    if (index < model->count) {
        (void)memmove(&model->items[index + 1U],
                      &model->items[index],
                      (model->count - index) * sizeof(model->items[0]));
    }
    model->items[index] = *item;
    model->count += 1U;
}

static void resequence_orders(UmiUiDocumentViewModel *model)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        size_t previous;
        int32_t order = 0;
        for (previous = 0U; previous < index; ++previous) {
            if (same_group(&model->items[previous], &model->items[index])) {
                order += 1;
            }
        }
        model->items[index].order = order;
    }
}

static UmiStatus copy_identifier(char *destination,
                                 size_t capacity,
                                 const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_create(UmiUiDocumentViewModel **out_model)
{
    UmiUiDocumentViewModel *model;
    UmiStatus status;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiDocumentViewModel *)calloc(1U, sizeof(*model));
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

void umi_ui_document_view_model_destroy(UmiUiDocumentViewModel *model)
{
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

UmiStatus umi_ui_document_view_model_upsert(
    UmiUiDocumentViewModel *model,
    const UmiUiDocumentViewSnapshot *item)
{
    size_t index;
    if (model == NULL || item == NULL ||
        !umi_ui_id_is_valid(item->view_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item->view_id);
    if (index == SIZE_MAX) {
        if (model->count >= UMI_UI_DOCUMENT_VIEW_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = model->count++;
    }
    model->items[index] = *item;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_remove(UmiUiDocumentViewModel *model,
                                             const char *id)
{
    size_t index;
    if (model == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    erase_at(model, index);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_find(
    const UmiUiDocumentViewModel *model,
    const char *id,
    UmiUiDocumentViewSnapshot *out_item)
{
    size_t index;
    if (model == NULL || id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_at(
    const UmiUiDocumentViewModel *model,
    size_t index,
    UmiUiDocumentViewSnapshot *out_item)
{
    if (model == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_document_view_model_count(const UmiUiDocumentViewModel *model)
{
    size_t count = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

uint64_t umi_ui_document_view_model_revision(
    const UmiUiDocumentViewModel *model)
{
    uint64_t revision = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}

UmiStatus umi_ui_document_view_model_open_preview(
    UmiUiDocumentViewModel *model,
    const UmiUiDocumentViewSnapshot *item)
{
    UmiUiDocumentViewSnapshot preview;
    size_t target;
    size_t index;
    if (model == NULL || item == NULL ||
        !umi_ui_id_is_valid(item->view_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    preview = *item;
    preview.preview = 1;
    preview.pinned = 0;
    preview.active = 1;

    (void)umi_mutex_lock(model->mutex);
    target = find_item(model, preview.view_id);
    if (target != SIZE_MAX) {
        for (index = 0U; index < model->count; ++index) {
            if (same_group(&model->items[index], &model->items[target])) {
                model->items[index].active = index == target;
            }
        }
        model->revision = umi_ui_next_revision(model->revision);
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_OK;
    }
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &preview) &&
            model->items[index].preview &&
            !model->items[index].dirty &&
            !model->items[index].pinned) {
            target = index;
            break;
        }
    }
    if (target == SIZE_MAX) {
        if (model->count >= UMI_UI_DOCUMENT_VIEW_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        target = model->count++;
    }
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &preview)) {
            model->items[index].active = 0;
        }
    }
    model->items[target] = preview;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_set_pinned(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int pinned)
{
    size_t index;
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].pinned = pinned != 0;
    if (pinned != 0) model->items[index].preview = 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_promote_preview(
    UmiUiDocumentViewModel *model,
    const char *item_id)
{
    size_t index;
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].preview = 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_set_word_wrap(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int word_wrap)
{
    size_t index;
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].word_wrap = word_wrap != 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_move_to_group(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    const char *group_id)
{
    size_t index;
    size_t other;
    UmiStatus status;
    if (model == NULL || item_id == NULL || group_id == NULL ||
        !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    status = copy_identifier(model->items[index].group_id,
                             sizeof(model->items[index].group_id),
                             group_id);
    if (status == UMI_STATUS_OK && model->items[index].active) {
        for (other = 0U; other < model->count; ++other) {
            if (other != index &&
                same_group(&model->items[other], &model->items[index])) {
                model->items[other].active = 0;
            }
        }
    }
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

UmiStatus umi_ui_document_view_model_place(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    const char *group_id,
    size_t position)
{
    UmiUiDocumentViewSnapshot item;
    UmiUiDocumentViewSnapshot requested = {0};
    size_t index;
    size_t insert_index;
    size_t group_position = 0U;
    size_t last_group_index = SIZE_MAX;
    size_t other;
    UmiStatus status;

    if (model == NULL || item_id == NULL || group_id == NULL ||
        !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(requested.group_id,
                             sizeof(requested.group_id), group_id);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    item = model->items[index];
    erase_at(model, index);
    status = copy_identifier(item.group_id, sizeof(item.group_id), group_id);
    if (status != UMI_STATUS_OK) {
        insert_at(model, index, &item);
        (void)umi_mutex_unlock(model->mutex);
        return status;
    }

    insert_index = model->count;
    for (other = 0U; other < model->count; ++other) {
        if (!same_group(&model->items[other], &requested)) continue;
        if (group_position == position) {
            insert_index = other;
            break;
        }
        last_group_index = other;
        group_position += 1U;
    }
    if (insert_index == model->count && last_group_index != SIZE_MAX) {
        insert_index = last_group_index + 1U;
    }
    insert_at(model, insert_index, &item);

    if (item.active) {
        for (other = 0U; other < model->count; ++other) {
            if (other != insert_index &&
                same_group(&model->items[other], &model->items[insert_index])) {
                model->items[other].active = 0;
            }
        }
    }
    resequence_orders(model);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_activate(
    UmiUiDocumentViewModel *model,
    const char *item_id)
{
    size_t index;
    size_t other;
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (other = 0U; other < model->count; ++other) {
        if (same_group(&model->items[other], &model->items[index])) {
            model->items[other].active = other == index;
        }
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_activate_relative(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int direction,
    char *out_item_id,
    size_t capacity)
{
    size_t current;
    size_t target;
    size_t index;
    size_t visited;
    UmiStatus status;
    if (model == NULL || item_id == NULL || direction == 0 ||
        out_item_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    current = find_item(model, item_id);
    if (current == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    target = current;
    for (visited = 0U; visited < model->count; ++visited) {
        target = direction > 0
            ? (target + 1U) % model->count
            : (target == 0U ? model->count - 1U : target - 1U);
        if (same_group(&model->items[target], &model->items[current])) break;
    }
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &model->items[current])) {
            model->items[index].active = index == target;
        }
    }
    status = copy_identifier(out_item_id, capacity,
                             model->items[target].view_id);
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

UmiStatus umi_ui_document_view_model_activate_group(
    UmiUiDocumentViewModel *model,
    const char *group_id,
    char *out_item_id,
    size_t capacity)
{
    UmiUiDocumentViewSnapshot requested = {0};
    size_t index;
    size_t target = SIZE_MAX;
    UmiStatus status;
    if (model == NULL || group_id == NULL || out_item_id == NULL ||
        capacity == 0U || !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(requested.group_id,
                             sizeof(requested.group_id),
                             group_id);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &requested)) {
            if (target == SIZE_MAX || model->items[index].active) {
                target = index;
            }
            if (model->items[index].active) break;
        }
    }
    if (target == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &requested)) {
            model->items[index].active = index == target;
        }
    }
    status = copy_identifier(out_item_id, capacity,
                             model->items[target].view_id);
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

UmiStatus umi_ui_document_view_model_merge_group(
    UmiUiDocumentViewModel *model,
    const char *source_group_id,
    const char *target_group_id)
{
    UmiUiDocumentViewSnapshot source = {0};
    UmiUiDocumentViewSnapshot target = {0};
    size_t index;
    int target_has_active = 0;
    int changed = 0;
    UmiStatus status;
    if (model == NULL || source_group_id == NULL || target_group_id == NULL ||
        !umi_ui_id_is_valid(source_group_id) ||
        !umi_ui_id_is_valid(target_group_id) ||
        strcmp(source_group_id, target_group_id) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(source.group_id, sizeof(source.group_id),
                             source_group_id);
    if (status == UMI_STATUS_OK) {
        status = copy_identifier(target.group_id, sizeof(target.group_id),
                                 target_group_id);
    }
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &target) &&
            model->items[index].active) {
            target_has_active = 1;
            break;
        }
    }
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &source)) {
            if (target_has_active && model->items[index].active) {
                model->items[index].active = 0;
            } else if (model->items[index].active) {
                target_has_active = 1;
            }
            status = copy_identifier(model->items[index].group_id,
                                     sizeof(model->items[index].group_id),
                                     target_group_id);
            if (status != UMI_STATUS_OK) {
                (void)umi_mutex_unlock(model->mutex);
                return status;
            }
            changed = 1;
        }
    }
    if (changed) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

static int should_close(const UmiUiDocumentViewSnapshot *item,
                        UmiUiDocumentCloseResult *result)
{
    if (item->dirty) {
        result->dirty_count += 1U;
        return 0;
    }
    if (item->pinned) {
        result->pinned_count += 1U;
        return 0;
    }
    if (!item->closable) {
        result->non_closable_count += 1U;
        return 0;
    }
    return 1;
}

UmiStatus umi_ui_document_view_model_close_others(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    UmiUiDocumentCloseResult *out_result)
{
    UmiUiDocumentCloseResult result = {0};
    size_t keep;
    size_t index = 0U;
    if (model == NULL || item_id == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    keep = find_item(model, item_id);
    if (keep == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    while (index < model->count) {
        if (strcmp(model->items[index].view_id, item_id) != 0 &&
            same_group(&model->items[index], &model->items[keep]) &&
            should_close(&model->items[index], &result)) {
            erase_at(model, index);
            result.closed_count += 1U;
            keep = find_item(model, item_id);
            continue;
        }
        ++index;
    }
    if (result.closed_count > 0U) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    *out_result = result;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_document_view_model_close_all(
    UmiUiDocumentViewModel *model,
    UmiUiDocumentCloseResult *out_result)
{
    UmiUiDocumentCloseResult result = {0};
    size_t index = 0U;
    if (model == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    while (index < model->count) {
        if (should_close(&model->items[index], &result)) {
            erase_at(model, index);
            result.closed_count += 1U;
            continue;
        }
        ++index;
    }
    if (result.closed_count > 0U) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    *out_result = result;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_document_view_model_group_count(
    const UmiUiDocumentViewModel *model,
    const char *group_id)
{
    UmiUiDocumentViewSnapshot requested = {0};
    size_t index;
    size_t count = 0U;
    if (model == NULL || group_id == NULL) return 0U;
    if (copy_identifier(requested.group_id, sizeof(requested.group_id),
                        group_id) != UMI_STATUS_OK) {
        return 0U;
    }
    (void)umi_mutex_lock(model->mutex);
    for (index = 0U; index < model->count; ++index) {
        if (same_group(&model->items[index], &requested)) ++count;
    }
    (void)umi_mutex_unlock(model->mutex);
    return count;
}
