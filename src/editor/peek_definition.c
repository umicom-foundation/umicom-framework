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

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static UmiStatus reserve_results(UmiEditorPeekDefinitionModel *model,
                                 size_t required)
{
    size_t capacity;
    UmiEditorNavigationResult *replacement;

    if (required <= model->capacity) return UMI_STATUS_OK;
    capacity = model->capacity > 0U ? model->capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorNavigationResult *)realloc(
        model->results, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->results = replacement;
    model->capacity = capacity;
    return UMI_STATUS_OK;
}

static int valid_query_kind(UmiEditorNavigationQueryKind kind)
{
    return kind == UMI_EDITOR_NAVIGATION_QUERY_DEFINITION ||
           kind == UMI_EDITOR_NAVIGATION_QUERY_DECLARATION ||
           kind == UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION ||
           kind == UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION;
}

UmiStatus umi_editor_peek_definition_model_create(
    UmiEditorPeekDefinitionModel **out_model)
{
    UmiEditorPeekDefinitionModel *model;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorPeekDefinitionModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->active_index = SIZE_MAX;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_editor_peek_definition_model_destroy(
    UmiEditorPeekDefinitionModel *model)
{
    if (model == NULL) return;
    free(model->results);
    model->results = NULL;
    free(model);
}

UmiStatus umi_editor_peek_definition_model_load(
    UmiEditorPeekDefinitionModel *model,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results)
{
    size_t count;
    size_t index;
    size_t selected_index;
    UmiStatus status;

    if (model == NULL || results == NULL ||
        umi_editor_source_location_validate(anchor) != UMI_STATUS_OK ||
        !valid_query_kind(
            umi_editor_navigation_result_set_query_kind(results))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_editor_navigation_result_set_count(results);
    status = reserve_results(model, count);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < count; ++index) {
        status = umi_editor_navigation_result_set_at(results,
                                                      index,
                                                      &model->results[index]);
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

UmiStatus umi_editor_peek_definition_model_show(
    UmiEditorPeekDefinitionModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count == 0U) return UMI_STATUS_NOT_FOUND;
    model->visible = 1;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_hide(
    UmiEditorPeekDefinitionModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->visible = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_set_pinned(
    UmiEditorPeekDefinitionModel *model,
    int pinned)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count == 0U && pinned) return UMI_STATUS_NOT_FOUND;
    model->pinned = pinned != 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_select(
    UmiEditorPeekDefinitionModel *model,
    size_t index)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    model->active_index = index;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_select_next(
    UmiEditorPeekDefinitionModel *model,
    int wrap)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count == 0U || model->active_index >= model->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (model->active_index + 1U < model->count) {
        model->active_index += 1U;
    } else if (wrap) {
        model->active_index = 0U;
    } else {
        return UMI_STATUS_NOT_FOUND;
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_select_previous(
    UmiEditorPeekDefinitionModel *model,
    int wrap)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count == 0U || model->active_index >= model->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (model->active_index > 0U) {
        model->active_index -= 1U;
    } else if (wrap) {
        model->active_index = model->count - 1U;
    } else {
        return UMI_STATUS_NOT_FOUND;
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_anchor(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorSourceLocation *out_anchor)
{
    if (model == NULL || out_anchor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!model->has_anchor) return UMI_STATUS_NOT_FOUND;
    *out_anchor = model->anchor;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_at(
    const UmiEditorPeekDefinitionModel *model,
    size_t index,
    UmiEditorNavigationResult *out_result)
{
    if (model == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_result = model->results[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_active(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorNavigationResult *out_result)
{
    if (model == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (model->active_index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_result = model->results[model->active_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_definition_model_snapshot(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorPeekDefinitionSnapshot *out_snapshot)
{
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

size_t umi_editor_peek_definition_model_count(
    const UmiEditorPeekDefinitionModel *model)
{
    return model != NULL ? model->count : 0U;
}

uint64_t umi_editor_peek_definition_model_revision(
    const UmiEditorPeekDefinitionModel *model)
{
    return model != NULL ? model->revision : 0U;
}
