/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/inlay_hint_model.c
 *
 * PURPOSE:
 *   Implement a dynamic, provider-neutral inline hint model shared by native,
 *   language-server and AI providers and every graphical frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/inlay_hint_model.h"

#include <stdlib.h>
#include <string.h>

#define INLAY_HINT_KIND_COUNT 6U

struct UmiEditorInlayHintModel {
    UmiEditorInlayHintItem *items;
    size_t count;
    size_t capacity;
    int kind_visible[INLAY_HINT_KIND_COUNT];
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static int valid_kind(UmiEditorInlayHintKind kind)
{
    return kind >= UMI_EDITOR_INLAY_HINT_PARAMETER &&
           kind <= UMI_EDITOR_INLAY_HINT_CUSTOM;
}

static UmiStatus validate_item(const UmiEditorInlayHintItem *item)
{
    if (item == NULL || item->struct_size != (uint32_t)sizeof(*item) ||
        item->api_version != UMI_EDITOR_INLAY_HINT_MODEL_API_VERSION ||
        !terminated(item->id, sizeof(item->id)) || item->id[0] == '\0' ||
        !terminated(item->provider_id, sizeof(item->provider_id)) ||
        item->provider_id[0] == '\0' ||
        !terminated(item->label, sizeof(item->label)) ||
        item->label[0] == '\0' ||
        !terminated(item->tooltip, sizeof(item->tooltip)) ||
        !valid_kind(item->kind) ||
        umi_editor_source_location_validate(&item->location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_items(UmiEditorInlayHintModel *model, size_t required)
{
    size_t capacity;
    UmiEditorInlayHintItem *replacement;

    if (required <= model->capacity) return UMI_STATUS_OK;
    capacity = model->capacity > 0U ? model->capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorInlayHintItem *)realloc(
        model->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->items = replacement;
    model->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_item(const UmiEditorInlayHintModel *model,
                        const char *item_id)
{
    size_t index;

    if (model == NULL || item_id == NULL) return SIZE_MAX;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].id, item_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int compare_items(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorInlayHintItem *left =
        (const UmiEditorInlayHintItem *)left_pointer;
    const UmiEditorInlayHintItem *right =
        (const UmiEditorInlayHintItem *)right_pointer;
    int order = umi_editor_source_location_compare(&left->location,
                                                   &right->location);
    if (order != 0) return order;
    if (left->kind < right->kind) return -1;
    if (left->kind > right->kind) return 1;
    return strcmp(left->id, right->id);
}

static int effectively_visible(const UmiEditorInlayHintModel *model,
                               const UmiEditorInlayHintItem *item)
{
    return item->visible && model->kind_visible[(size_t)item->kind];
}

UmiStatus umi_editor_inlay_hint_model_create(UmiEditorInlayHintModel **out_model)
{
    UmiEditorInlayHintModel *model;
    size_t index;

    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorInlayHintModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (index = 0U; index < INLAY_HINT_KIND_COUNT; ++index) {
        model->kind_visible[index] = 1;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_editor_inlay_hint_model_destroy(UmiEditorInlayHintModel *model)
{
    if (model == NULL) return;
    free(model->items);
    model->items = NULL;
    free(model);
}

UmiStatus umi_editor_inlay_hint_model_clear(UmiEditorInlayHintModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->count = 0U;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inlay_hint_model_upsert(
    UmiEditorInlayHintModel *model,
    const UmiEditorInlayHintItem *item)
{
    UmiEditorInlayHintItem stored;
    size_t index;
    UmiStatus status;

    if (model == NULL || validate_item(item) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(model, item->id);
    if (index == SIZE_MAX) {
        status = reserve_items(model, model->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = model->count++;
    }
    stored = *item;
    stored.padding_before = item->padding_before != 0;
    stored.padding_after = item->padding_after != 0;
    stored.resolved = item->resolved != 0;
    stored.visible = item->visible != 0;
    model->items[index] = stored;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inlay_hint_model_remove(
    UmiEditorInlayHintModel *model,
    const char *item_id)
{
    size_t index;

    if (model == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(model, item_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index], &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(*model->items));
    }
    --model->count;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inlay_hint_model_set_kind_visible(
    UmiEditorInlayHintModel *model,
    UmiEditorInlayHintKind kind,
    int visible)
{
    if (model == NULL || !valid_kind(kind)) return UMI_STATUS_INVALID_ARGUMENT;
    model->kind_visible[(size_t)kind] = visible != 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inlay_hint_model_sort(UmiEditorInlayHintModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count > 1U) {
        qsort(model->items, model->count, sizeof(*model->items), compare_items);
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inlay_hint_model_at(
    const UmiEditorInlayHintModel *model,
    size_t index,
    UmiEditorInlayHintItem *out_item)
{
    if (model == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_item = model->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inlay_hint_model_visible_at(
    const UmiEditorInlayHintModel *model,
    size_t visible_index,
    UmiEditorInlayHintItem *out_item)
{
    size_t index;
    size_t position = 0U;

    if (model == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->count; ++index) {
        if (!effectively_visible(model, &model->items[index])) continue;
        if (position++ == visible_index) {
            *out_item = model->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_inlay_hint_model_snapshot(
    const UmiEditorInlayHintModel *model,
    UmiEditorInlayHintSnapshot *out_snapshot)
{
    size_t index;

    if (model == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_INLAY_HINT_MODEL_API_VERSION;
    out_snapshot->item_count = model->count;
    out_snapshot->revision = model->revision;
    for (index = 0U; index < model->count; ++index) {
        const UmiEditorInlayHintItem *item = &model->items[index];
        if (effectively_visible(model, item)) ++out_snapshot->visible_item_count;
        if (item->resolved) ++out_snapshot->resolved_item_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_inlay_hint_model_count(const UmiEditorInlayHintModel *model)
{
    return model != NULL ? model->count : 0U;
}

uint64_t umi_editor_inlay_hint_model_revision(const UmiEditorInlayHintModel *model)
{
    return model != NULL ? model->revision : 0U;
}
