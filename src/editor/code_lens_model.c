/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/code_lens_model.c
 *
 * PURPOSE:
 *   Implement a dynamic, provider-neutral CodeLens model with stable ordering
 *   and independent visibility controls for each CodeLens category.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/code_lens_model.h"

#include <stdlib.h>
#include <string.h>

#define CODE_LENS_KIND_COUNT 7U

struct UmiEditorCodeLensModel {
    UmiEditorCodeLensItem *items;
    size_t count;
    size_t capacity;
    int kind_visible[CODE_LENS_KIND_COUNT];
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
static int valid_kind(UmiEditorCodeLensKind kind)
{
    return kind >= UMI_EDITOR_CODE_LENS_REFERENCE &&
           kind <= UMI_EDITOR_CODE_LENS_CUSTOM;
}

/* Provide the validate item operation used by this module and its client applications. */
static UmiStatus validate_item(const UmiEditorCodeLensItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || item->struct_size != (uint32_t)sizeof(*item) ||
        item->api_version != UMI_EDITOR_CODE_LENS_MODEL_API_VERSION ||
        !terminated(item->id, sizeof(item->id)) || item->id[0] == '\0' ||
        !terminated(item->provider_id, sizeof(item->provider_id)) ||
        item->provider_id[0] == '\0' ||
        !terminated(item->title, sizeof(item->title)) ||
        !terminated(item->command_id, sizeof(item->command_id)) ||
        !terminated(item->argument, sizeof(item->argument)) ||
        !valid_kind(item->kind) ||
        item->resolution_state < UMI_EDITOR_CODE_LENS_UNRESOLVED ||
        item->resolution_state > UMI_EDITOR_CODE_LENS_FAILED ||
        umi_editor_source_location_validate(&item->location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->resolution_state == UMI_EDITOR_CODE_LENS_RESOLVED &&
        (item->title[0] == '\0' || item->command_id[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiEditorCodeLensModel *model, size_t required)
{
    size_t capacity;
    UmiEditorCodeLensItem *replacement;

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
    replacement = (UmiEditorCodeLensItem *)realloc(
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
static size_t find_item(const UmiEditorCodeLensModel *model,
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
    const UmiEditorCodeLensItem *left =
        (const UmiEditorCodeLensItem *)left_pointer;
    const UmiEditorCodeLensItem *right =
        (const UmiEditorCodeLensItem *)right_pointer;
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
static int effectively_visible(const UmiEditorCodeLensModel *model,
                               const UmiEditorCodeLensItem *item)
{
    return item->visible && model->kind_visible[(size_t)item->kind];
}

/*
 * Initialise editor code lens model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_code_lens_model_create(UmiEditorCodeLensModel **out_model)
{
    UmiEditorCodeLensModel *model;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorCodeLensModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < CODE_LENS_KIND_COUNT; ++index) {
        model->kind_visible[index] = 1;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor code lens model so the same storage can be reused
 * safely.
 */
void umi_editor_code_lens_model_destroy(UmiEditorCodeLensModel *model)
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
 * Release or reset state held by editor code lens model so the same storage can be reused
 * safely.
 */
UmiStatus umi_editor_code_lens_model_clear(UmiEditorCodeLensModel *model)
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
 * Provide the editor code lens model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_code_lens_model_upsert(
    UmiEditorCodeLensModel *model,
    const UmiEditorCodeLensItem *item)
{
    UmiEditorCodeLensItem stored;
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
    stored.enabled = item->enabled != 0;
    stored.visible = item->visible != 0;
    model->items[index] = stored;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove editor code lens model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_code_lens_model_remove(
    UmiEditorCodeLensModel *model,
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
 * Provide the editor code lens model set kind visible operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_code_lens_model_set_kind_visible(
    UmiEditorCodeLensModel *model,
    UmiEditorCodeLensKind kind,
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
 * Provide the editor code lens model sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_code_lens_model_sort(UmiEditorCodeLensModel *model)
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
 * Find editor code lens model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_code_lens_model_at(
    const UmiEditorCodeLensModel *model,
    size_t index,
    UmiEditorCodeLensItem *out_item)
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
 * Find editor code lens model visible while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_code_lens_model_visible_at(
    const UmiEditorCodeLensModel *model,
    size_t visible_index,
    UmiEditorCodeLensItem *out_item)
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
 * Provide the editor code lens model snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_code_lens_model_snapshot(
    const UmiEditorCodeLensModel *model,
    UmiEditorCodeLensSnapshot *out_snapshot)
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
    out_snapshot->api_version = UMI_EDITOR_CODE_LENS_MODEL_API_VERSION;
    out_snapshot->item_count = model->count;
    out_snapshot->revision = model->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        const UmiEditorCodeLensItem *item = &model->items[index];
        /* Apply this operation only while the related capability or state is available. */
        if (effectively_visible(model, item)) ++out_snapshot->visible_item_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->resolution_state == UMI_EDITOR_CODE_LENS_RESOLVED) {
            ++out_snapshot->resolved_item_count;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (item->resolution_state == UMI_EDITOR_CODE_LENS_UNRESOLVED) {
            ++out_snapshot->unresolved_item_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor code lens model without changing
 * their state.
 */
size_t umi_editor_code_lens_model_count(const UmiEditorCodeLensModel *model)
{
    return model != NULL ? model->count : 0U;
}

/*
 * Provide the editor code lens model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_code_lens_model_revision(const UmiEditorCodeLensModel *model)
{
    return model != NULL ? model->revision : 0U;
}
