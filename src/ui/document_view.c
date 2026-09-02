/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/document_view.c
 *
 * PURPOSE:
 *   Implement the toolkit-neutral document-tab model including preview,
 *   grouping, navigation, activation, close safety and pinning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiDocumentViewModel *model, const char *id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].view_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the effective group operation used by this module and its client applications. */
static const char *effective_group(const UmiUiDocumentViewSnapshot *item)
{
    return item->group_id[0] != '\0'
        ? item->group_id
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

/* Provide the same group operation used by this module and its client applications. */
static int same_group(const UmiUiDocumentViewSnapshot *left,
                      const UmiUiDocumentViewSnapshot *right)
{
    return strcmp(effective_group(left), effective_group(right)) == 0;
}

/* Find erase while leaving the underlying catalogue or model owned by this module. */
static void erase_at(UmiUiDocumentViewModel *model, size_t index)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index],
                      &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(model->items[0]));
    }
    model->count -= 1U;
}

/* Find insert while leaving the underlying catalogue or model owned by this module. */
static void insert_at(UmiUiDocumentViewModel *model,
                      size_t index,
                      const UmiUiDocumentViewSnapshot *item)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index < model->count) {
        (void)memmove(&model->items[index + 1U],
                      &model->items[index],
                      (model->count - index) * sizeof(model->items[0]));
    }
    model->items[index] = *item;
    model->count += 1U;
}

/* Provide the resequence orders operation used by this module and its client applications. */
static void resequence_orders(UmiUiDocumentViewModel *model)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        size_t previous;
        int32_t order = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (same_group(&model->items[previous], &model->items[index])) {
                order += 1;
            }
        }
        model->items[index].order = order;
    }
}

/* Provide the copy identifier operation used by this module and its client applications. */
static UmiStatus copy_identifier(char *destination,
                                 size_t capacity,
                                 const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ui document view model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_document_view_model_create(UmiUiDocumentViewModel **out_model)
{
    UmiUiDocumentViewModel *model;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiDocumentViewModel *)calloc(1U, sizeof(*model));
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
 * Release or reset state held by ui document view model so the same storage can be reused
 * safely.
 */
void umi_ui_document_view_model_destroy(UmiUiDocumentViewModel *model)
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
 * Provide the ui document view model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_view_model_upsert(
    UmiUiDocumentViewModel *model,
    const UmiUiDocumentViewSnapshot *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item == NULL ||
        !umi_ui_id_is_valid(item->view_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item->view_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Remove ui document view model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_document_view_model_remove(UmiUiDocumentViewModel *model,
                                             const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    erase_at(model, index);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find ui document view model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_document_view_model_find(
    const UmiUiDocumentViewModel *model,
    const char *id,
    UmiUiDocumentViewSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find ui document view model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_document_view_model_at(
    const UmiUiDocumentViewModel *model,
    size_t index,
    UmiUiDocumentViewSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ui document view model without changing
 * their state.
 */
size_t umi_ui_document_view_model_count(const UmiUiDocumentViewModel *model)
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
 * Provide the ui document view model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_document_view_model_revision(
    const UmiUiDocumentViewModel *model)
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

/*
 * Provide the ui document view model open preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_open_preview(
    UmiUiDocumentViewModel *model,
    const UmiUiDocumentViewSnapshot *item)
{
    UmiUiDocumentViewSnapshot preview;
    size_t target;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Configure the optional target only when its feature has created it. */
    if (target != SIZE_MAX) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < model->count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (same_group(&model->items[index], &model->items[target])) {
                model->items[index].active = index == target;
            }
        }
        model->revision = umi_ui_next_revision(model->revision);
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_OK;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &preview) &&
            model->items[index].preview &&
            !model->items[index].dirty &&
            !model->items[index].pinned) {
            target = index;
            break;
        }
    }
    /* Configure the optional target only when its feature has created it. */
    if (target == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count >= UMI_UI_DOCUMENT_VIEW_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        target = model->count++;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &preview)) {
            model->items[index].active = 0;
        }
    }
    model->items[target] = preview;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model set pinned operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_set_pinned(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int pinned)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].pinned = pinned != 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (pinned != 0) model->items[index].preview = 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model promote preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_promote_preview(
    UmiUiDocumentViewModel *model,
    const char *item_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].preview = 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model set word wrap operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_set_word_wrap(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int word_wrap)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].word_wrap = word_wrap != 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model move to group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_move_to_group(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    const char *group_id)
{
    size_t index;
    size_t other;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || group_id == NULL ||
        !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    status = copy_identifier(model->items[index].group_id,
                             sizeof(model->items[index].group_id),
                             group_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK && model->items[index].active) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (other = 0U; other < model->count; ++other) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (other != index &&
                same_group(&model->items[other], &model->items[index])) {
                model->items[other].active = 0;
            }
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/*
 * Provide the ui document view model place operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || group_id == NULL ||
        !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(requested.group_id,
                             sizeof(requested.group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    item = model->items[index];
    erase_at(model, index);
    status = copy_identifier(item.group_id, sizeof(item.group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        insert_at(model, index, &item);
        (void)umi_mutex_unlock(model->mutex);
        return status;
    }

    insert_index = model->count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (other = 0U; other < model->count; ++other) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!same_group(&model->items[other], &requested)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (group_position == position) {
            insert_index = other;
            break;
        }
        last_group_index = other;
        group_position += 1U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (insert_index == model->count && last_group_index != SIZE_MAX) {
        insert_index = last_group_index + 1U;
    }
    insert_at(model, insert_index, &item);

    /* Apply this operation only while the related capability or state is available. */
    if (item.active) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (other = 0U; other < model->count; ++other) {
            /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the ui document view model activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_view_model_activate(
    UmiUiDocumentViewModel *model,
    const char *item_id)
{
    size_t index;
    size_t other;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (other = 0U; other < model->count; ++other) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[other], &model->items[index])) {
            model->items[other].active = other == index;
        }
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model activate relative operation used by this module and
 * its client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || direction == 0 ||
        out_item_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    current = find_item(model, item_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (current == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    target = current;
    /* Visit each bounded item once so every record receives the same rule. */
    for (visited = 0U; visited < model->count; ++visited) {
        target = direction > 0
            ? (target + 1U) % model->count
            : (target == 0U ? model->count - 1U : target - 1U);
        /* Apply this branch only when its contract condition is satisfied. */
        if (same_group(&model->items[target], &model->items[current])) break;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &model->items[current])) {
            model->items[index].active = index == target;
        }
    }
    status = copy_identifier(out_item_id, capacity,
                             model->items[target].view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/*
 * Provide the ui document view model activate group operation used by this module and its
 * client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || group_id == NULL || out_item_id == NULL ||
        capacity == 0U || !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(requested.group_id,
                             sizeof(requested.group_id),
                             group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &requested)) {
            /* Configure the optional target only when its feature has created it. */
            if (target == SIZE_MAX || model->items[index].active) {
                target = index;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (model->items[index].active) break;
        }
    }
    /* Configure the optional target only when its feature has created it. */
    if (target == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &requested)) {
            model->items[index].active = index == target;
        }
    }
    status = copy_identifier(out_item_id, capacity,
                             model->items[target].view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/*
 * Provide the ui document view model merge group operation used by this module and its
 * client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || source_group_id == NULL || target_group_id == NULL ||
        !umi_ui_id_is_valid(source_group_id) ||
        !umi_ui_id_is_valid(target_group_id) ||
        strcmp(source_group_id, target_group_id) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(source.group_id, sizeof(source.group_id),
                             source_group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_identifier(target.group_id, sizeof(target.group_id),
                                 target_group_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &target) &&
            model->items[index].active) {
            target_has_active = 1;
            break;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &source)) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (target_has_active && model->items[index].active) {
                model->items[index].active = 0;
            } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (model->items[index].active) {
                target_has_active = 1;
            }
            status = copy_identifier(model->items[index].group_id,
                                     sizeof(model->items[index].group_id),
                                     target_group_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                (void)umi_mutex_unlock(model->mutex);
                return status;
            }
            changed = 1;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (changed) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/* Provide the should close operation used by this module and its client applications. */
static int should_close(const UmiUiDocumentViewSnapshot *item,
                        UmiUiDocumentCloseResult *result)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->dirty) {
        result->dirty_count += 1U;
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->pinned) {
        result->pinned_count += 1U;
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!item->closable) {
        result->non_closable_count += 1U;
        return 0;
    }
    return 1;
}

/*
 * Provide the ui document view model close others operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_close_others(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    UmiUiDocumentCloseResult *out_result)
{
    UmiUiDocumentCloseResult result = {0};
    size_t keep;
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    keep = find_item(model, item_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (keep == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < model->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.closed_count > 0U) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    *out_result = result;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model close all operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_close_all(
    UmiUiDocumentViewModel *model,
    UmiUiDocumentCloseResult *out_result)
{
    UmiUiDocumentCloseResult result = {0};
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < model->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (should_close(&model->items[index], &result)) {
            erase_at(model, index);
            result.closed_count += 1U;
            continue;
        }
        ++index;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.closed_count > 0U) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    *out_result = result;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ui document view model group without
 * changing their state.
 */
size_t umi_ui_document_view_model_group_count(
    const UmiUiDocumentViewModel *model,
    const char *group_id)
{
    UmiUiDocumentViewSnapshot requested = {0};
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || group_id == NULL) return 0U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (copy_identifier(requested.group_id, sizeof(requested.group_id),
                        group_id) != UMI_STATUS_OK) {
        return 0U;
    }
    (void)umi_mutex_lock(model->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &requested)) ++count;
    }
    (void)umi_mutex_unlock(model->mutex);
    return count;
}
