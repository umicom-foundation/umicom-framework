/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/inlay_hint_model.c
 *
 * PURPOSE:
 *   Implement a dynamic, provider-neutral inline hint model shared by native,
 *   language-server and AI providers and every graphical frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the valid kind operation used by this module and its client applications. */
static int valid_kind(UmiEditorInlayHintKind kind)
{
    return kind >= UMI_EDITOR_INLAY_HINT_PARAMETER &&
           kind <= UMI_EDITOR_INLAY_HINT_CUSTOM;
}

/* Provide the validate item operation used by this module and its client applications. */
static UmiStatus validate_item(const UmiEditorInlayHintItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiEditorInlayHintModel *model, size_t required)
{
    size_t capacity;
    UmiEditorInlayHintItem *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= model->capacity) return UMI_STATUS_OK;
    capacity = model->capacity > 0U ? model->capacity : 16U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorInlayHintItem *)realloc(
        model->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->items = replacement;
    model->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiEditorInlayHintModel *model,
                        const char *item_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].id, item_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the compare items operation used by this module and its client applications. */
static int compare_items(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorInlayHintItem *left =
        (const UmiEditorInlayHintItem *)left_pointer;
    const UmiEditorInlayHintItem *right =
        (const UmiEditorInlayHintItem *)right_pointer;
    int order = umi_editor_source_location_compare(&left->location,
                                                   &right->location);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->kind < right->kind) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->kind > right->kind) return 1;
    return strcmp(left->id, right->id);
}

/*
 * Provide the effectively visible operation used by this module and its client
 * applications.
 */
static int effectively_visible(const UmiEditorInlayHintModel *model,
                               const UmiEditorInlayHintItem *item)
{
    return item->visible && model->kind_visible[(size_t)item->kind];
}

/*
 * Initialise editor inlay hint model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_inlay_hint_model_create(UmiEditorInlayHintModel **out_model)
{
    UmiEditorInlayHintModel *model;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorInlayHintModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < INLAY_HINT_KIND_COUNT; ++index) {
        model->kind_visible[index] = 1;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor inlay hint model so the same storage can be reused
 * safely.
 */
void umi_editor_inlay_hint_model_destroy(UmiEditorInlayHintModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    free(model->items);
    model->items = NULL;
    free(model);
}

/*
 * Release or reset state held by editor inlay hint model so the same storage can be reused
 * safely.
 */
UmiStatus umi_editor_inlay_hint_model_clear(UmiEditorInlayHintModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->count = 0U;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inlay hint model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_inlay_hint_model_upsert(
    UmiEditorInlayHintModel *model,
    const UmiEditorInlayHintItem *item)
{
    UmiEditorInlayHintItem stored;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || validate_item(item) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(model, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        status = reserve_items(model, model->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Remove editor inlay hint model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_inlay_hint_model_remove(
    UmiEditorInlayHintModel *model,
    const char *item_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index], &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(*model->items));
    }
    --model->count;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inlay hint model set kind visible operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inlay_hint_model_set_kind_visible(
    UmiEditorInlayHintModel *model,
    UmiEditorInlayHintKind kind,
    int visible)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !valid_kind(kind)) return UMI_STATUS_INVALID_ARGUMENT;
    model->kind_visible[(size_t)kind] = visible != 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inlay hint model sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_inlay_hint_model_sort(UmiEditorInlayHintModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count > 1U) {
        qsort(model->items, model->count, sizeof(*model->items), compare_items);
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor inlay hint model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_inlay_hint_model_at(
    const UmiEditorInlayHintModel *model,
    size_t index,
    UmiEditorInlayHintItem *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_item = model->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find editor inlay hint model visible while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_inlay_hint_model_visible_at(
    const UmiEditorInlayHintModel *model,
    size_t visible_index,
    UmiEditorInlayHintItem *out_item)
{
    size_t index;
    size_t position = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!effectively_visible(model, &model->items[index])) continue;
        /* Apply this operation only while the related capability or state is available. */
        if (position++ == visible_index) {
            *out_item = model->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the editor inlay hint model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_inlay_hint_model_snapshot(
    const UmiEditorInlayHintModel *model,
    UmiEditorInlayHintSnapshot *out_snapshot)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_INLAY_HINT_MODEL_API_VERSION;
    out_snapshot->item_count = model->count;
    out_snapshot->revision = model->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        const UmiEditorInlayHintItem *item = &model->items[index];
        /* Apply this operation only while the related capability or state is available. */
        if (effectively_visible(model, item)) ++out_snapshot->visible_item_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->resolved) ++out_snapshot->resolved_item_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor inlay hint model without changing
 * their state.
 */
size_t umi_editor_inlay_hint_model_count(const UmiEditorInlayHintModel *model)
{
    return model != NULL ? model->count : 0U;
}

/*
 * Provide the editor inlay hint model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_inlay_hint_model_revision(const UmiEditorInlayHintModel *model)
{
    return model != NULL ? model->revision : 0U;
}
