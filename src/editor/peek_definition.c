/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/peek_definition.c
 *
 * PURPOSE:
 *   Implement a dynamic peek-definition result model suitable for editor
 *   overlays, detached windows and future web frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/peek_definition.h"

#include <stdlib.h>

struct UmiEditorPeekDefinitionModel {
    UmiEditorNavigationResult *results;
    size_t count;
    size_t capacity;
    size_t active_index;
    UmiEditorSourceLocation anchor;
    uint64_t revision;
    int visible;
    int pinned;
    int has_anchor;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the reserve results operation used by this module and its client applications. */
static UmiStatus reserve_results(UmiEditorPeekDefinitionModel *model,
                                 size_t required)
{
    size_t capacity;
    UmiEditorNavigationResult *replacement;

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
    replacement = (UmiEditorNavigationResult *)realloc(
        model->results, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->results = replacement;
    model->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the valid query kind operation used by this module and its client applications. */
static int valid_query_kind(UmiEditorNavigationQueryKind kind)
{
    return kind == UMI_EDITOR_NAVIGATION_QUERY_DEFINITION ||
           kind == UMI_EDITOR_NAVIGATION_QUERY_DECLARATION ||
           kind == UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION ||
           kind == UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION;
}

/*
 * Initialise editor peek definition model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_peek_definition_model_create(
    UmiEditorPeekDefinitionModel **out_model)
{
    UmiEditorPeekDefinitionModel *model;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorPeekDefinitionModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->active_index = SIZE_MAX;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor peek definition model so the same storage can be
 * reused safely.
 */
void umi_editor_peek_definition_model_destroy(
    UmiEditorPeekDefinitionModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    free(model->results);
    model->results = NULL;
    free(model);
}

/*
 * Read editor peek definition model into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_editor_peek_definition_model_load(
    UmiEditorPeekDefinitionModel *model,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results)
{
    size_t count;
    size_t index;
    size_t selected_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || results == NULL ||
        umi_editor_source_location_validate(anchor) != UMI_STATUS_OK ||
        !valid_query_kind(
            umi_editor_navigation_result_set_query_kind(results))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_editor_navigation_result_set_count(results);
    status = reserve_results(model, count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        status = umi_editor_navigation_result_set_at(results,
                                                      index,
                                                      &model->results[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    selected_index = umi_editor_navigation_result_set_selected_index(results);
    model->count = count;
    model->active_index = selected_index < count ? selected_index
                                                  : (count > 0U ? 0U : SIZE_MAX);
    model->anchor = *anchor;
    model->has_anchor = 1;
    model->visible = count > 0U;
    model->pinned = 0;
    model->revision = next_revision(model->revision);
    return count > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the editor peek definition model show operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_show(
    UmiEditorPeekDefinitionModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count == 0U) return UMI_STATUS_NOT_FOUND;
    model->visible = 1;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model hide operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_hide(
    UmiEditorPeekDefinitionModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->visible = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model set pinned operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_definition_model_set_pinned(
    UmiEditorPeekDefinitionModel *model,
    int pinned)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count == 0U && pinned) return UMI_STATUS_NOT_FOUND;
    model->pinned = pinned != 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_select(
    UmiEditorPeekDefinitionModel *model,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    model->active_index = index;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_definition_model_select_next(
    UmiEditorPeekDefinitionModel *model,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count == 0U || model->active_index >= model->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->active_index + 1U < model->count) {
        model->active_index += 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (wrap) {
        model->active_index = 0U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_NOT_FOUND;
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_definition_model_select_previous(
    UmiEditorPeekDefinitionModel *model,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count == 0U || model->active_index >= model->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (model->active_index > 0U) {
        model->active_index -= 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (wrap) {
        model->active_index = model->count - 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_NOT_FOUND;
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model anchor operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_anchor(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorSourceLocation *out_anchor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_anchor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!model->has_anchor) return UMI_STATUS_NOT_FOUND;
    *out_anchor = model->anchor;
    return UMI_STATUS_OK;
}

/*
 * Find editor peek definition model while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_peek_definition_model_at(
    const UmiEditorPeekDefinitionModel *model,
    size_t index,
    UmiEditorNavigationResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_result = model->results[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_active(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorNavigationResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->active_index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_result = model->results[model->active_index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek definition model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_snapshot(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorPeekDefinitionSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_PEEK_DEFINITION_API_VERSION;
    out_snapshot->result_count = model->count;
    out_snapshot->active_index = model->active_index < model->count
        ? model->active_index
        : 0U;
    out_snapshot->revision = model->revision;
    out_snapshot->visible = model->visible;
    out_snapshot->pinned = model->pinned;
    out_snapshot->has_anchor = model->has_anchor;
    out_snapshot->can_select_previous = model->count > 1U;
    out_snapshot->can_select_next = model->count > 1U;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor peek definition model without
 * changing their state.
 */
size_t umi_editor_peek_definition_model_count(
    const UmiEditorPeekDefinitionModel *model)
{
    return model != NULL ? model->count : 0U;
}

/*
 * Provide the editor peek definition model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_peek_definition_model_revision(
    const UmiEditorPeekDefinitionModel *model)
{
    return model != NULL ? model->revision : 0U;
}
