/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/signature_help_model.c
 *
 * PURPOSE:
 *   Implement overload ownership, trigger context and deterministic active
 *   signature navigation without depending on LSP or a graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/signature_help_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorSignatureHelpModel {
    UmiEditorSignatureHelpItem *items;
    size_t count;
    size_t capacity;
    char active_signature_id[UMI_EDITOR_SIGNATURE_ID_CAPACITY];
    UmiEditorSignatureHelpContext context;
    uint64_t revision;
    int presented;
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

/*
 * Provide the valid trigger kind operation used by this module and its client
 * applications.
 */
static int valid_trigger_kind(UmiEditorSignatureHelpTriggerKind kind)
{
    return kind >= UMI_EDITOR_SIGNATURE_HELP_INVOKED &&
           kind <= UMI_EDITOR_SIGNATURE_HELP_RETRIGGER;
}

/* Provide the validate item operation used by this module and its client applications. */
static UmiStatus validate_item(const UmiEditorSignatureHelpItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || item->struct_size != (uint32_t)sizeof(*item) ||
        item->api_version != UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION ||
        !terminated(item->id, sizeof(item->id)) || item->id[0] == '\0' ||
        !terminated(item->provider_id, sizeof(item->provider_id)) ||
        item->provider_id[0] == '\0' ||
        !terminated(item->label, sizeof(item->label)) ||
        item->label[0] == '\0' ||
        !terminated(item->documentation, sizeof(item->documentation)) ||
        !terminated(item->return_type, sizeof(item->return_type)) ||
        umi_editor_source_location_validate(&item->location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the validate context operation used by this module and its client applications. */
static UmiStatus validate_context(const UmiEditorSignatureHelpContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL ||
        context->struct_size != (uint32_t)sizeof(*context) ||
        context->api_version != UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION ||
        !valid_trigger_kind(context->trigger_kind)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context->trigger_kind == UMI_EDITOR_SIGNATURE_HELP_TRIGGER_CHARACTER &&
        context->trigger_character == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiEditorSignatureHelpModel *model,
                               size_t required)
{
    size_t capacity;
    UmiEditorSignatureHelpItem *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= model->capacity) return UMI_STATUS_OK;
    capacity = model->capacity > 0U ? model->capacity : 8U;
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
    replacement = (UmiEditorSignatureHelpItem *)realloc(
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
static size_t find_item(const UmiEditorSignatureHelpModel *model,
                        const char *signature_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || signature_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].id, signature_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the compare items operation used by this module and its client applications. */
static int compare_items(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorSignatureHelpItem *left =
        (const UmiEditorSignatureHelpItem *)left_pointer;
    const UmiEditorSignatureHelpItem *right =
        (const UmiEditorSignatureHelpItem *)right_pointer;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score > right->score) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score < right->score) return 1;
    return strcmp(left->id, right->id);
}

/* Provide the visible index of operation used by this module and its client applications. */
static size_t visible_index_of(const UmiEditorSignatureHelpModel *model,
                               const char *signature_id)
{
    size_t index;
    size_t visible_index = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!model->items[index].visible) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].id, signature_id) == 0) {
            return visible_index;
        }
        ++visible_index;
    }
    return SIZE_MAX;
}

/*
 * Provide the visible model index operation used by this module and its client
 * applications.
 */
static size_t visible_model_index(const UmiEditorSignatureHelpModel *model,
                                  size_t visible_index)
{
    size_t index;
    size_t position = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!model->items[index].visible) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (position++ == visible_index) return index;
    }
    return SIZE_MAX;
}

/* Return the number of records represented by visible without changing their state. */
static size_t visible_count(const UmiEditorSignatureHelpModel *model)
{
    size_t index;
    size_t count = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->items[index].visible) ++count;
    }
    return count;
}

/* Provide the select visible operation used by this module and its client applications. */
static UmiStatus select_visible(UmiEditorSignatureHelpModel *model,
                                int direction,
                                int wrap)
{
    size_t count;
    size_t current;
    size_t target;
    size_t model_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    count = visible_count(model);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    current = visible_index_of(model, model->active_signature_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (current == SIZE_MAX) {
        target = direction > 0 ? 0U : count - 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (direction > 0) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (current + 1U < count) {
            target = current + 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (wrap != 0) {
            target = 0U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            return UMI_STATUS_NOT_FOUND;
        }
    } else /* Apply this branch only when its contract condition is satisfied. */ if (current > 0U) {
        target = current - 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (wrap != 0) {
        target = count - 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_NOT_FOUND;
    }
    model_index = visible_model_index(model, target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (model_index == SIZE_MAX) return UMI_STATUS_INTERNAL_ERROR;
    (void)memcpy(model->active_signature_id, model->items[model_index].id,
                 strlen(model->items[model_index].id) + 1U);
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Initialise editor signature help model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_signature_help_model_create(
    UmiEditorSignatureHelpModel **out_model)
{
    UmiEditorSignatureHelpModel *model;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorSignatureHelpModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->context.struct_size = (uint32_t)sizeof(model->context);
    model->context.api_version = UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION;
    model->context.trigger_kind = UMI_EDITOR_SIGNATURE_HELP_INVOKED;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor signature help model so the same storage can be
 * reused safely.
 */
void umi_editor_signature_help_model_destroy(UmiEditorSignatureHelpModel *model)
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
 * Release or reset state held by editor signature help model so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_signature_help_model_clear(
    UmiEditorSignatureHelpModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->count = 0U;
    model->active_signature_id[0] = '\0';
    model->presented = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor signature help model upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_upsert(
    UmiEditorSignatureHelpModel *model,
    const UmiEditorSignatureHelpItem *item)
{
    UmiEditorSignatureHelpItem stored;
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
    stored.deprecated = item->deprecated != 0;
    stored.visible = item->visible != 0;
    model->items[index] = stored;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (model->active_signature_id[0] == '\0' && stored.visible) {
        (void)memcpy(model->active_signature_id, stored.id,
                     strlen(stored.id) + 1U);
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove editor signature help model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_signature_help_model_remove(
    UmiEditorSignatureHelpModel *model,
    const char *signature_id)
{
    size_t index;
    int removed_active;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || signature_id == NULL || signature_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(model, signature_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    removed_active = strcmp(model->active_signature_id, signature_id) == 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index], &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(*model->items));
    }
    --model->count;
    /* Apply this operation only while the related capability or state is available. */
    if (removed_active) {
        model->active_signature_id[0] = '\0';
        (void)select_visible(model, 1, 1);
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count == 0U) model->presented = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor signature help model sort operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_sort(
    UmiEditorSignatureHelpModel *model)
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
 * Provide the editor signature help model set context operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_signature_help_model_set_context(
    UmiEditorSignatureHelpModel *model,
    const UmiEditorSignatureHelpContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || validate_context(context) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->context = *context;
    model->context.has_active_parameter = context->has_active_parameter != 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor signature help model set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_set_active(
    UmiEditorSignatureHelpModel *model,
    const char *signature_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || signature_id == NULL || signature_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(model, signature_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!model->items[index].visible) return UMI_STATUS_INVALID_STATE;
    (void)memcpy(model->active_signature_id, signature_id,
                 strlen(signature_id) + 1U);
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor signature help model select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_signature_help_model_select_next(
    UmiEditorSignatureHelpModel *model,
    int wrap)
{
    return select_visible(model, 1, wrap);
}

/*
 * Provide the editor signature help model select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_signature_help_model_select_previous(
    UmiEditorSignatureHelpModel *model,
    int wrap)
{
    return select_visible(model, -1, wrap);
}

/*
 * Provide the editor signature help model set presented operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_signature_help_model_set_presented(
    UmiEditorSignatureHelpModel *model,
    int presented)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (presented != 0 && visible_count(model) == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    model->presented = presented != 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor signature help model while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_signature_help_model_at(
    const UmiEditorSignatureHelpModel *model,
    size_t index,
    UmiEditorSignatureHelpItem *out_item)
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
 * Provide the editor signature help model active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_active(
    const UmiEditorSignatureHelpModel *model,
    UmiEditorSignatureHelpItem *out_item)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_item(model, model->active_signature_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = model->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor signature help model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_snapshot(
    const UmiEditorSignatureHelpModel *model,
    UmiEditorSignatureHelpSnapshot *out_snapshot)
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
    out_snapshot->api_version = UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION;
    out_snapshot->signature_count = model->count;
    out_snapshot->visible_signature_count = visible_count(model);
    out_snapshot->revision = model->revision;
    out_snapshot->context = model->context;
    out_snapshot->presented = model->presented;
    out_snapshot->active_index =
        visible_index_of(model, model->active_signature_id);
    out_snapshot->has_active_signature =
        out_snapshot->active_index != SIZE_MAX;
    (void)memcpy(out_snapshot->active_signature_id,
                 model->active_signature_id,
                 strlen(model->active_signature_id) + 1U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->items[index].deprecated) {
            ++out_snapshot->deprecated_signature_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor signature help model without changing
 * their state.
 */
size_t umi_editor_signature_help_model_count(
    const UmiEditorSignatureHelpModel *model)
{
    return model != NULL ? model->count : 0U;
}

/*
 * Provide the editor signature help model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_signature_help_model_revision(
    const UmiEditorSignatureHelpModel *model)
{
    return model != NULL ? model->revision : 0U;
}
