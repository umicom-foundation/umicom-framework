/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/linked_editing_model.c
 *
 * PURPOSE:
 *   Validate, order and synchronize linked ranges through Framework workspace
 *   edit sets with overlap detection and document revision preservation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/linked_editing_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorLinkedEditingModel {
    UmiEditorLinkedRange *ranges;
    size_t count;
    size_t capacity;
    char active_group_id[UMI_EDITOR_LINKED_GROUP_ID_CAPACITY];
    uint64_t revision;
    int finalized;
    int has_active_group;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_range(const UmiEditorLinkedRange *range)
{
    uint64_t span;
    size_t text_length;

    if (range == NULL || range->struct_size != (uint32_t)sizeof(*range) ||
        range->api_version != UMI_EDITOR_LINKED_EDITING_API_VERSION ||
        !terminated(range->id, sizeof(range->id)) || range->id[0] == '\0' ||
        !terminated(range->group_id, sizeof(range->group_id)) ||
        range->group_id[0] == '\0' ||
        !terminated(range->provider_id, sizeof(range->provider_id)) ||
        range->provider_id[0] == '\0' ||
        !terminated(range->original_text, sizeof(range->original_text)) ||
        umi_editor_source_location_validate(&range->location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    span = range->location.end_byte_offset - range->location.byte_offset;
    text_length = strlen(range->original_text);
    if (span == 0U || span > SIZE_MAX || (size_t)span != text_length) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_ranges(UmiEditorLinkedEditingModel *model,
                                size_t required)
{
    size_t capacity;
    UmiEditorLinkedRange *replacement;

    if (required <= model->capacity) return UMI_STATUS_OK;
    capacity = model->capacity > 0U ? model->capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorLinkedRange *)realloc(
        model->ranges, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->ranges = replacement;
    model->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_range(const UmiEditorLinkedEditingModel *model,
                         const char *range_id)
{
    size_t index;

    if (model == NULL || range_id == NULL) return SIZE_MAX;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->ranges[index].id, range_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int compare_ranges(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorLinkedRange *left =
        (const UmiEditorLinkedRange *)left_pointer;
    const UmiEditorLinkedRange *right =
        (const UmiEditorLinkedRange *)right_pointer;
    int order = strcmp(left->location.uri, right->location.uri);

    if (order != 0) return order;
    if (left->location.byte_offset < right->location.byte_offset) return -1;
    if (left->location.byte_offset > right->location.byte_offset) return 1;
    if (left->location.end_byte_offset < right->location.end_byte_offset) {
        return -1;
    }
    if (left->location.end_byte_offset > right->location.end_byte_offset) {
        return 1;
    }
    return strcmp(left->id, right->id);
}

static int groups_equal(const char *left, const char *right)
{
    return strcmp(left, right) == 0;
}

UmiStatus umi_editor_linked_editing_model_create(
    UmiEditorLinkedEditingModel **out_model)
{
    UmiEditorLinkedEditingModel *model;

    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorLinkedEditingModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_editor_linked_editing_model_destroy(UmiEditorLinkedEditingModel *model)
{
    if (model == NULL) return;
    free(model->ranges);
    model->ranges = NULL;
    free(model);
}

UmiStatus umi_editor_linked_editing_model_clear(
    UmiEditorLinkedEditingModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->count = 0U;
    model->active_group_id[0] = '\0';
    model->has_active_group = 0;
    model->finalized = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_linked_editing_model_upsert(
    UmiEditorLinkedEditingModel *model,
    const UmiEditorLinkedRange *range)
{
    UmiEditorLinkedRange stored;
    size_t index;
    UmiStatus status;

    if (model == NULL || validate_range(range) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_range(model, range->id);
    if (index == SIZE_MAX) {
        status = reserve_ranges(model, model->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = model->count++;
    }
    stored = *range;
    stored.primary = range->primary != 0;
    stored.editable = range->editable != 0;
    stored.conflict = 0;
    model->ranges[index] = stored;
    model->finalized = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_linked_editing_model_remove(
    UmiEditorLinkedEditingModel *model,
    const char *range_id)
{
    size_t index;

    if (model == NULL || range_id == NULL || range_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_range(model, range_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < model->count) {
        (void)memmove(&model->ranges[index], &model->ranges[index + 1U],
                      (model->count - index - 1U) * sizeof(*model->ranges));
    }
    --model->count;
    model->finalized = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_linked_editing_model_remove_group(
    UmiEditorLinkedEditingModel *model,
    const char *group_id)
{
    size_t read_index;
    size_t write_index = 0U;
    size_t removed = 0U;

    if (model == NULL || group_id == NULL || group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (read_index = 0U; read_index < model->count; ++read_index) {
        if (groups_equal(model->ranges[read_index].group_id, group_id)) {
            ++removed;
            continue;
        }
        if (write_index != read_index) {
            model->ranges[write_index] = model->ranges[read_index];
        }
        ++write_index;
    }
    if (removed == 0U) return UMI_STATUS_NOT_FOUND;
    model->count = write_index;
    if (model->has_active_group &&
        groups_equal(model->active_group_id, group_id)) {
        model->active_group_id[0] = '\0';
        model->has_active_group = 0;
    }
    model->finalized = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_linked_editing_model_finalize(
    UmiEditorLinkedEditingModel *model)
{
    size_t index;
    size_t comparison;
    size_t conflict_count = 0U;

    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count > 1U) {
        qsort(model->ranges, model->count, sizeof(*model->ranges),
              compare_ranges);
    }
    for (index = 0U; index < model->count; ++index) {
        model->ranges[index].conflict = 0;
    }
    for (index = 0U; index < model->count; ++index) {
        UmiEditorLinkedRange *current = &model->ranges[index];
        for (comparison = index + 1U; comparison < model->count;
             ++comparison) {
            UmiEditorLinkedRange *candidate = &model->ranges[comparison];
            if (strcmp(current->location.uri, candidate->location.uri) != 0 ||
                candidate->location.byte_offset >=
                    current->location.end_byte_offset) {
                break;
            }
            current->conflict = 1;
            candidate->conflict = 1;
        }
    }
    for (index = 0U; index < model->count; ++index) {
        if (model->ranges[index].conflict) ++conflict_count;
    }
    model->finalized = 1;
    model->revision = next_revision(model->revision);
    return conflict_count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_editor_linked_editing_model_set_active_group(
    UmiEditorLinkedEditingModel *model,
    const char *group_id)
{
    size_t index;
    size_t length;

    if (model == NULL || group_id == NULL || group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(group_id);
    if (length >= sizeof(model->active_group_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < model->count; ++index) {
        if (groups_equal(model->ranges[index].group_id, group_id)) break;
    }
    if (index == model->count) return UMI_STATUS_NOT_FOUND;
    (void)memcpy(model->active_group_id, group_id, length + 1U);
    model->has_active_group = 1;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_linked_editing_model_clear_active_group(
    UmiEditorLinkedEditingModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->active_group_id[0] = '\0';
    model->has_active_group = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_linked_editing_model_build_edit_set(
    const UmiEditorLinkedEditingModel *model,
    const char *replacement_text,
    UmiEditorWorkspaceEditSet *edit_set)
{
    size_t index;
    size_t replacement_length;
    size_t added = 0U;
    UmiStatus status;

    if (model == NULL || replacement_text == NULL || edit_set == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!model->finalized || !model->has_active_group) {
        return UMI_STATUS_INVALID_STATE;
    }
    replacement_length = strlen(replacement_text);
    if (replacement_length >= UMI_EDITOR_WORKSPACE_EDIT_TEXT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < model->count; ++index) {
        const UmiEditorLinkedRange *range = &model->ranges[index];

        if (!groups_equal(range->group_id, model->active_group_id) ||
            !range->editable) {
            continue;
        }
        if (range->conflict) return UMI_STATUS_INVALID_STATE;
        ++added;
    }
    if (added == 0U) return UMI_STATUS_NOT_FOUND;
    added = 0U;
    status = umi_editor_workspace_edit_set_clear(edit_set);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < model->count; ++index) {
        const UmiEditorLinkedRange *range = &model->ranges[index];
        UmiEditorWorkspaceTextEdit edit;

        if (!groups_equal(range->group_id, model->active_group_id) ||
            !range->editable) {
            continue;
        }
        (void)memset(&edit, 0, sizeof(edit));
        edit.struct_size = (uint32_t)sizeof(edit);
        edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
        (void)memcpy(edit.id, range->id, strlen(range->id) + 1U);
        (void)memcpy(edit.provider_id, range->provider_id,
                     strlen(range->provider_id) + 1U);
        edit.location = range->location;
        (void)memcpy(edit.expected_text, range->original_text,
                     strlen(range->original_text) + 1U);
        (void)memcpy(edit.replacement_text, replacement_text,
                     replacement_length + 1U);
        edit.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
        edit.required = 1;
        status = umi_editor_workspace_edit_set_upsert(edit_set, &edit);
        if (status != UMI_STATUS_OK) return status;
        ++added;
    }
    return umi_editor_workspace_edit_set_finalize(edit_set);
}

UmiStatus umi_editor_linked_editing_model_at(
    const UmiEditorLinkedEditingModel *model,
    size_t index,
    UmiEditorLinkedRange *out_range)
{
    if (model == NULL || out_range == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_range = model->ranges[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_linked_editing_model_active_group_at(
    const UmiEditorLinkedEditingModel *model,
    size_t index,
    UmiEditorLinkedRange *out_range)
{
    size_t position = 0U;
    size_t range_index;

    if (model == NULL || out_range == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!model->has_active_group) return UMI_STATUS_INVALID_STATE;
    for (range_index = 0U; range_index < model->count; ++range_index) {
        if (!groups_equal(model->ranges[range_index].group_id,
                          model->active_group_id)) {
            continue;
        }
        if (position++ == index) {
            *out_range = model->ranges[range_index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_linked_editing_model_snapshot(
    const UmiEditorLinkedEditingModel *model,
    UmiEditorLinkedEditingSnapshot *out_snapshot)
{
    size_t index;
    size_t comparison;

    if (model == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_LINKED_EDITING_API_VERSION;
    out_snapshot->range_count = model->count;
    out_snapshot->revision = model->revision;
    out_snapshot->finalized = model->finalized;
    out_snapshot->has_active_group = model->has_active_group;
    (void)memcpy(out_snapshot->active_group_id, model->active_group_id,
                 sizeof(out_snapshot->active_group_id));
    for (index = 0U; index < model->count; ++index) {
        int first_group = 1;
        if (model->ranges[index].editable) ++out_snapshot->editable_count;
        if (model->ranges[index].conflict) ++out_snapshot->conflict_count;
        if (model->has_active_group &&
            groups_equal(model->ranges[index].group_id,
                         model->active_group_id)) {
            ++out_snapshot->active_group_range_count;
        }
        for (comparison = 0U; comparison < index; ++comparison) {
            if (groups_equal(model->ranges[index].group_id,
                             model->ranges[comparison].group_id)) {
                first_group = 0;
                break;
            }
        }
        if (first_group) ++out_snapshot->group_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_linked_editing_model_count(
    const UmiEditorLinkedEditingModel *model)
{
    return model != NULL ? model->count : 0U;
}

uint64_t umi_editor_linked_editing_model_revision(
    const UmiEditorLinkedEditingModel *model)
{
    return model != NULL ? model->revision : 0U;
}
