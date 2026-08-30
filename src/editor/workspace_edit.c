/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_edit.c
 *
 * PURPOSE:
 *   Implement deterministic workspace edit planning, overlap detection,
 *   protocol-coordinate resolution and revision-safe text-buffer application.
 *
 * ARCHITECTURE:
 *   Strict native edits retain their expected-text/revision guard. LSP and
 *   other coordinate-only producers can stage UNRESOLVED edits and resolve
 *   those ranges against the current Framework text buffer before applying.
 *   This keeps protocol adaptation in Framework without weakening safety.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

static UmiStatus validate_edit_common(
    const UmiEditorWorkspaceTextEdit *edit,
    int allow_unresolved)
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
        edit->state > UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED ||
        umi_editor_source_location_validate(&edit->location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (edit->state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
        if (!allow_unresolved || edit->expected_text[0] != '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        return UMI_STATUS_OK;
    }

    span = edit->location.end_byte_offset - edit->location.byte_offset;
    if (span > SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    expected_length = strlen(edit->expected_text);
    if ((size_t)span != expected_length) {
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

static int compare_uint64(uint64_t left, uint64_t right)
{
    if (left < right) return -1;
    if (left > right) return 1;
    return 0;
}

static int compare_edits(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorWorkspaceTextEdit *left =
        (const UmiEditorWorkspaceTextEdit *)left_pointer;
    const UmiEditorWorkspaceTextEdit *right =
        (const UmiEditorWorkspaceTextEdit *)right_pointer;
    int order = strcmp(left->location.uri, right->location.uri);

    if (order != 0) return order;
    order = compare_uint64(left->location.line, right->location.line);
    if (order != 0) return order;
    order = compare_uint64(left->location.column, right->location.column);
    if (order != 0) return order;
    order = compare_uint64(left->location.end_line, right->location.end_line);
    if (order != 0) return order;
    order = compare_uint64(left->location.end_column, right->location.end_column);
    if (order != 0) return order;
    order = compare_uint64(
        left->location.byte_offset, right->location.byte_offset);
    if (order != 0) return order;
    order = compare_uint64(
        left->location.end_byte_offset, right->location.end_byte_offset);
    if (order != 0) return order;
    return strcmp(left->id, right->id);
}

static UmiStatus upsert_internal(
    UmiEditorWorkspaceEditSet *edit_set,
    const UmiEditorWorkspaceTextEdit *edit,
    int allow_unresolved)
{
    size_t index;
    UmiStatus status;

    if (edit_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_edit_common(edit, allow_unresolved);
    if (status != UMI_STATUS_OK) return status;

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
    if (edit != NULL && edit->state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return upsert_internal(edit_set, edit, 0);
}

UmiStatus umi_editor_workspace_edit_set_upsert_unresolved(
    UmiEditorWorkspaceEditSet *edit_set,
    const UmiEditorWorkspaceTextEdit *edit)
{
    if (edit == NULL || edit->state != UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return upsert_internal(edit_set, edit, 1);
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
        if (edit_set->items[index].state != UMI_EDITOR_WORKSPACE_EDIT_APPLIED &&
            edit_set->items[index].state != UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
            edit_set->items[index].state = UMI_EDITOR_WORKSPACE_EDIT_READY;
        }
    }

    for (index = 0U; index < edit_set->count; ++index) {
        UmiEditorWorkspaceTextEdit *current = &edit_set->items[index];

        if (current->state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED ||
            current->state == UMI_EDITOR_WORKSPACE_EDIT_APPLIED) {
            continue;
        }

        for (comparison = index + 1U; comparison < edit_set->count;
             ++comparison) {
            UmiEditorWorkspaceTextEdit *candidate =
                &edit_set->items[comparison];

            if (strcmp(current->location.uri, candidate->location.uri) != 0) {
                break;
            }
            if (candidate->state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED ||
                candidate->state == UMI_EDITOR_WORKSPACE_EDIT_APPLIED) {
                continue;
            }
            if (candidate->location.byte_offset >=
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

/*
 * Decode one UTF-8 scalar and report the number of LSP UTF-16 code units it
 * occupies. Four-byte Unicode scalars consume a surrogate pair (two units).
 */
static UmiStatus utf8_measure_scalar(
    const unsigned char *bytes,
    size_t remaining,
    size_t *out_bytes,
    uint32_t *out_utf16_units)
{
    unsigned char first;
    uint32_t scalar;
    size_t width;
    size_t index;

    if (bytes == NULL || out_bytes == NULL || out_utf16_units == NULL ||
        remaining == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    first = bytes[0];
    if (first < 0x80U) {
        *out_bytes = 1U;
        *out_utf16_units = 1U;
        return UMI_STATUS_OK;
    }

    if ((first & 0xE0U) == 0xC0U) {
        width = 2U;
        scalar = (uint32_t)(first & 0x1FU);
        if (first < 0xC2U) return UMI_STATUS_PARSE_ERROR;
    } else if ((first & 0xF0U) == 0xE0U) {
        width = 3U;
        scalar = (uint32_t)(first & 0x0FU);
    } else if ((first & 0xF8U) == 0xF0U) {
        width = 4U;
        scalar = (uint32_t)(first & 0x07U);
        if (first > 0xF4U) return UMI_STATUS_PARSE_ERROR;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }

    if (remaining < width) return UMI_STATUS_PARSE_ERROR;
    for (index = 1U; index < width; ++index) {
        if ((bytes[index] & 0xC0U) != 0x80U) return UMI_STATUS_PARSE_ERROR;
        scalar = (scalar << 6U) | (uint32_t)(bytes[index] & 0x3FU);
    }

    if ((width == 3U && scalar < 0x800U) ||
        (width == 4U && scalar < 0x10000U) ||
        (scalar >= 0xD800U && scalar <= 0xDFFFU) ||
        scalar > 0x10FFFFU) {
        return UMI_STATUS_PARSE_ERROR;
    }

    *out_bytes = width;
    *out_utf16_units = scalar > 0xFFFFU ? 2U : 1U;
    return UMI_STATUS_OK;
}

static UmiStatus resolve_lsp_position(
    const UmiEditorTextBufferView *view,
    uint64_t target_line,
    uint64_t target_character,
    size_t *out_offset)
{
    size_t offset = 0U;
    uint64_t line = 0U;
    uint64_t character = 0U;

    if (view == NULL || out_offset == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (line < target_line) {
        while (offset < view->byte_count && view->bytes[offset] != '\n') {
            ++offset;
        }
        if (offset >= view->byte_count) return UMI_STATUS_INVALID_ARGUMENT;
        ++offset;
        ++line;
    }

    while (character < target_character) {
        size_t scalar_bytes;
        uint32_t utf16_units;
        UmiStatus status;
        unsigned char current;

        if (offset >= view->byte_count) return UMI_STATUS_INVALID_ARGUMENT;
        current = (unsigned char)view->bytes[offset];
        if (current == (unsigned char)'\n' || current == (unsigned char)'\r') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        status = utf8_measure_scalar(
            (const unsigned char *)view->bytes + offset,
            view->byte_count - offset,
            &scalar_bytes,
            &utf16_units);
        if (status != UMI_STATUS_OK) return status;
        if (utf16_units > target_character - character) {
            /* A UTF-16 range may not bisect a surrogate pair. */
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        offset += scalar_bytes;
        character += utf16_units;
    }

    *out_offset = offset;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_edit_set_resolve_document(
    UmiEditorWorkspaceEditSet *edit_set,
    const char *document_uri,
    const UmiEditorTextBuffer *buffer)
{
    UmiEditorTextBufferView view;
    UmiEditorWorkspaceTextEdit *resolved_items;
    size_t index;
    size_t resolved_count = 0U;
    UmiStatus status;

    if (edit_set == NULL || document_uri == NULL || document_uri[0] == '\0' ||
        buffer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_editor_text_buffer_view(buffer, &view);
    if (status != UMI_STATUS_OK) return status;

    if (edit_set->count == 0U) return UMI_STATUS_NOT_FOUND;
    if (edit_set->count > SIZE_MAX / sizeof(*resolved_items)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    resolved_items = (UmiEditorWorkspaceTextEdit *)malloc(
        edit_set->count * sizeof(*resolved_items));
    if (resolved_items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(
        resolved_items,
        edit_set->items,
        edit_set->count * sizeof(*resolved_items));

    for (index = 0U; index < edit_set->count; ++index) {
        UmiEditorWorkspaceTextEdit *edit = &resolved_items[index];
        size_t start_offset;
        size_t end_offset;
        size_t span;

        if (strcmp(edit->location.uri, document_uri) != 0 ||
            edit->state != UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
            continue;
        }

        status = resolve_lsp_position(
            &view,
            edit->location.line,
            edit->location.column,
            &start_offset);
        if (status != UMI_STATUS_OK) goto failure;

        status = resolve_lsp_position(
            &view,
            edit->location.end_line,
            edit->location.end_column,
            &end_offset);
        if (status != UMI_STATUS_OK) goto failure;

        if (end_offset < start_offset) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            goto failure;
        }

        span = end_offset - start_offset;
        if (span >= sizeof(edit->expected_text)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto failure;
        }

        if (span > 0U) {
            (void)memcpy(
                edit->expected_text,
                view.bytes + start_offset,
                span);
        }
        edit->expected_text[span] = '\0';
        edit->location.byte_offset = (uint64_t)start_offset;
        edit->location.end_byte_offset = (uint64_t)end_offset;
        edit->location.document_revision = view.revision;
        edit->state = UMI_EDITOR_WORKSPACE_EDIT_READY;
        ++resolved_count;
    }

    if (resolved_count == 0U) {
        free(resolved_items);
        return UMI_STATUS_NOT_FOUND;
    }

    (void)memcpy(
        edit_set->items,
        resolved_items,
        edit_set->count * sizeof(*resolved_items));
    free(resolved_items);

    edit_set->finalized = 0;
    edit_set->revision = next_revision(edit_set->revision);
    return umi_editor_workspace_edit_set_finalize(edit_set);

failure:
    free(resolved_items);
    return status;
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

    for (index = 0U; index < edit_set->count; ++index) {
        if (strcmp(edit_set->items[index].location.uri, document_uri) == 0 &&
            edit_set->items[index].state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
            return UMI_STATUS_INVALID_STATE;
        }
    }

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
    size_t unresolved_count = 0U;

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
        } else if (edit->state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
            ++unresolved_count;
        }
    }

    out_snapshot->applicable = edit_set->finalized && ready_count > 0U &&
                               unresolved_count == 0U &&
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
