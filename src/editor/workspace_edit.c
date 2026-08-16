/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_edit.c
 *
 * PURPOSE:
 *   Implement deterministic workspace edit planning, overlap detection and
 *   revision-safe application to Framework text buffers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_edit.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorWorkspaceEditSet {
    UmiEditorWorkspaceTextEdit *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
    int finalized;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_edit(const UmiEditorWorkspaceTextEdit *edit)
{
    uint64_t span;
    size_t expected_length;

    if (edit == NULL || edit->struct_size != (uint32_t)sizeof(*edit) ||
        edit->api_version != UMI_EDITOR_WORKSPACE_EDIT_API_VERSION ||
        !terminated(edit->id, sizeof(edit->id)) || edit->id[0] == '\0' ||
        !terminated(edit->provider_id, sizeof(edit->provider_id)) ||
        !terminated(edit->expected_text, sizeof(edit->expected_text)) ||
        !terminated(edit->replacement_text, sizeof(edit->replacement_text)) ||
        edit->state < UMI_EDITOR_WORKSPACE_EDIT_READY ||
        edit->state > UMI_EDITOR_WORKSPACE_EDIT_SKIPPED ||
        umi_editor_source_location_validate(&edit->location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    span = edit->location.end_byte_offset - edit->location.byte_offset;
    expected_length = strlen(edit->expected_text);
    if (span == 0U || span > SIZE_MAX || (size_t)span != expected_length) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_edits(UmiEditorWorkspaceEditSet *edit_set,
                               size_t required)
{
    size_t capacity;
    UmiEditorWorkspaceTextEdit *replacement;

    if (required <= edit_set->capacity) return UMI_STATUS_OK;
    capacity = edit_set->capacity > 0U ? edit_set->capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorWorkspaceTextEdit *)realloc(
        edit_set->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    edit_set->items = replacement;
    edit_set->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_edit(const UmiEditorWorkspaceEditSet *edit_set,
                        const char *edit_id)
{
    size_t index;

    if (edit_set == NULL || edit_id == NULL) return SIZE_MAX;
    for (index = 0U; index < edit_set->count; ++index) {
        if (strcmp(edit_set->items[index].id, edit_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int compare_edits(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorWorkspaceTextEdit *left =
        (const UmiEditorWorkspaceTextEdit *)left_pointer;
    const UmiEditorWorkspaceTextEdit *right =
        (const UmiEditorWorkspaceTextEdit *)right_pointer;
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

UmiStatus umi_editor_workspace_edit_set_create(
    UmiEditorWorkspaceEditSet **out_edit_set)
{
    UmiEditorWorkspaceEditSet *edit_set;

    if (out_edit_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_edit_set = NULL;
    edit_set = (UmiEditorWorkspaceEditSet *)calloc(1U, sizeof(*edit_set));
    if (edit_set == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    edit_set->revision = 1U;
    *out_edit_set = edit_set;
    return UMI_STATUS_OK;
}

void umi_editor_workspace_edit_set_destroy(UmiEditorWorkspaceEditSet *edit_set)
{
    if (edit_set == NULL) return;
    free(edit_set->items);
    edit_set->items = NULL;
    free(edit_set);
}

UmiStatus umi_editor_workspace_edit_set_clear(
    UmiEditorWorkspaceEditSet *edit_set)
{
    if (edit_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    edit_set->count = 0U;
    edit_set->finalized = 0;
    edit_set->revision = next_revision(edit_set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_edit_set_upsert(
    UmiEditorWorkspaceEditSet *edit_set,
    const UmiEditorWorkspaceTextEdit *edit)
{
    size_t index;
    UmiStatus status;

    if (edit_set == NULL || validate_edit(edit) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_edit(edit_set, edit->id);
    if (index == SIZE_MAX) {
        status = reserve_edits(edit_set, edit_set->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = edit_set->count++;
    }
    edit_set->items[index] = *edit;
    edit_set->finalized = 0;
    edit_set->revision = next_revision(edit_set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_edit_set_remove(
    UmiEditorWorkspaceEditSet *edit_set,
    const char *edit_id)
{
    size_t index;

    if (edit_set == NULL || edit_id == NULL || edit_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_edit(edit_set, edit_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < edit_set->count) {
        (void)memmove(&edit_set->items[index],
                      &edit_set->items[index + 1U],
                      (edit_set->count - index - 1U) * sizeof(*edit_set->items));
    }
    --edit_set->count;
    edit_set->finalized = 0;
    edit_set->revision = next_revision(edit_set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_edit_set_finalize(
    UmiEditorWorkspaceEditSet *edit_set)
{
    size_t index;
    size_t comparison;

    if (edit_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (edit_set->count > 1U) {
        qsort(edit_set->items, edit_set->count, sizeof(*edit_set->items),
              compare_edits);
    }
    for (index = 0U; index < edit_set->count; ++index) {
        if (edit_set->items[index].state != UMI_EDITOR_WORKSPACE_EDIT_APPLIED) {
            edit_set->items[index].state = UMI_EDITOR_WORKSPACE_EDIT_READY;
        }
    }
    for (index = 0U; index < edit_set->count; ++index) {
        UmiEditorWorkspaceTextEdit *current = &edit_set->items[index];
        for (comparison = index + 1U; comparison < edit_set->count;
             ++comparison) {
            UmiEditorWorkspaceTextEdit *candidate =
                &edit_set->items[comparison];
            if (strcmp(current->location.uri, candidate->location.uri) != 0 ||
                candidate->location.byte_offset >=
                    current->location.end_byte_offset) {
                break;
            }
            current->state = UMI_EDITOR_WORKSPACE_EDIT_CONFLICT;
            candidate->state = UMI_EDITOR_WORKSPACE_EDIT_CONFLICT;
        }
    }
    edit_set->finalized = 1;
    edit_set->revision = next_revision(edit_set->revision);
    return UMI_STATUS_OK;
}

static void mark_document_conflicts(UmiEditorWorkspaceEditSet *edit_set,
                                    const char *document_uri)
{
    size_t index;

    for (index = 0U; index < edit_set->count; ++index) {
        UmiEditorWorkspaceTextEdit *edit = &edit_set->items[index];
        if (strcmp(edit->location.uri, document_uri) == 0 &&
            edit->state == UMI_EDITOR_WORKSPACE_EDIT_READY) {
            edit->state = UMI_EDITOR_WORKSPACE_EDIT_CONFLICT;
        }
    }
    edit_set->revision = next_revision(edit_set->revision);
}

UmiStatus umi_editor_workspace_edit_set_apply_document(
    UmiEditorWorkspaceEditSet *edit_set,
    const char *document_uri,
    UmiEditorTextBuffer *buffer,
    int require_matching_revision,
    size_t *out_applied_count)
{
    UmiEditorTextBufferView view;
    size_t index;
    size_t first = SIZE_MAX;
    size_t last = SIZE_MAX;
    size_t final_size;
    UmiStatus status;

    if (out_applied_count != NULL) *out_applied_count = 0U;
    if (edit_set == NULL || document_uri == NULL || document_uri[0] == '\0' ||
        buffer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!edit_set->finalized) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_text_buffer_view(buffer, &view);
    if (status != UMI_STATUS_OK) return status;
    final_size = view.byte_count;
    for (index = 0U; index < edit_set->count; ++index) {
        UmiEditorWorkspaceTextEdit *edit = &edit_set->items[index];
        size_t start;
        size_t removed;
        size_t inserted;
        if (strcmp(edit->location.uri, document_uri) != 0 ||
            edit->state != UMI_EDITOR_WORKSPACE_EDIT_READY) {
            continue;
        }
        if (first == SIZE_MAX) first = index;
        last = index;
        if (edit->location.byte_offset > SIZE_MAX ||
            edit->location.end_byte_offset > SIZE_MAX) {
            mark_document_conflicts(edit_set, document_uri);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        start = (size_t)edit->location.byte_offset;
        removed = (size_t)(edit->location.end_byte_offset -
                           edit->location.byte_offset);
        inserted = strlen(edit->replacement_text);
        if (start > view.byte_count || removed > view.byte_count - start ||
            memcmp(view.bytes + start, edit->expected_text, removed) != 0 ||
            (require_matching_revision &&
             edit->location.document_revision != 0U &&
             edit->location.document_revision != view.revision)) {
            mark_document_conflicts(edit_set, document_uri);
            return UMI_STATUS_INVALID_STATE;
        }
        if (inserted >= removed) {
            size_t growth = inserted - removed;
            if (growth > SIZE_MAX - final_size) {
                mark_document_conflicts(edit_set, document_uri);
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            final_size += growth;
        } else {
            final_size -= removed - inserted;
        }
    }
    if (first == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (final_size == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_editor_text_buffer_reserve(buffer, final_size + 1U);
    if (status != UMI_STATUS_OK) return status;
    index = last + 1U;
    while (index > first) {
        UmiEditorWorkspaceTextEdit *edit;
        size_t removed;
        --index;
        edit = &edit_set->items[index];
        if (strcmp(edit->location.uri, document_uri) != 0 ||
            edit->state != UMI_EDITOR_WORKSPACE_EDIT_READY) {
            continue;
        }
        removed = (size_t)(edit->location.end_byte_offset -
                           edit->location.byte_offset);
        status = umi_editor_text_buffer_replace(
            buffer,
            (size_t)edit->location.byte_offset,
            removed,
            edit->replacement_text,
            strlen(edit->replacement_text));
        if (status != UMI_STATUS_OK) return status;
        edit->state = UMI_EDITOR_WORKSPACE_EDIT_APPLIED;
        if (out_applied_count != NULL) ++(*out_applied_count);
    }
    edit_set->revision = next_revision(edit_set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_edit_set_at(
    const UmiEditorWorkspaceEditSet *edit_set,
    size_t index,
    UmiEditorWorkspaceTextEdit *out_edit)
{
    if (edit_set == NULL || out_edit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= edit_set->count) return UMI_STATUS_NOT_FOUND;
    *out_edit = edit_set->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_edit_set_snapshot(
    const UmiEditorWorkspaceEditSet *edit_set,
    UmiEditorWorkspaceEditSnapshot *out_snapshot)
{
    size_t index;
    size_t comparison;
    size_t ready_count = 0U;

    if (edit_set == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    out_snapshot->edit_count = edit_set->count;
    out_snapshot->revision = edit_set->revision;
    out_snapshot->finalized = edit_set->finalized;
    for (index = 0U; index < edit_set->count; ++index) {
        const UmiEditorWorkspaceTextEdit *edit = &edit_set->items[index];
        int first_document_occurrence = 1;
        for (comparison = 0U; comparison < index; ++comparison) {
            if (strcmp(edit->location.uri,
                       edit_set->items[comparison].location.uri) == 0) {
                first_document_occurrence = 0;
                break;
            }
        }
        if (first_document_occurrence) {
            ++out_snapshot->document_count;
        }
        if (edit->state == UMI_EDITOR_WORKSPACE_EDIT_CONFLICT) {
            ++out_snapshot->conflict_count;
        } else if (edit->state == UMI_EDITOR_WORKSPACE_EDIT_APPLIED) {
            ++out_snapshot->applied_count;
        } else if (edit->state == UMI_EDITOR_WORKSPACE_EDIT_READY) {
            ++ready_count;
        }
    }
    out_snapshot->applicable = edit_set->finalized && ready_count > 0U &&
                               out_snapshot->conflict_count == 0U;
    return UMI_STATUS_OK;
}

size_t umi_editor_workspace_edit_set_count(
    const UmiEditorWorkspaceEditSet *edit_set)
{
    return edit_set != NULL ? edit_set->count : 0U;
}

uint64_t umi_editor_workspace_edit_set_revision(
    const UmiEditorWorkspaceEditSet *edit_set)
{
    return edit_set != NULL ? edit_set->revision : 0U;
}
