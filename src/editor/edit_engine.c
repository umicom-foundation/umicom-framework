/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/edit_engine.c
 *
 * PURPOSE:
 *   Implement atomic professional editing operations over Framework storage.
 *   Full before/after command transactions make multi-line edits undoable as
 *   one action while a configurable memory budget bounds retained history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/edit_engine.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiEditorCommandTransaction {
    UmiEditorEditCommandKind kind;
    char *before_bytes;
    size_t before_count;
    char *after_bytes;
    size_t after_count;
    size_t before_cursor;
    size_t after_cursor;
    size_t affected_line_count;
    size_t removed_byte_count;
    size_t inserted_byte_count;
} UmiEditorCommandTransaction;

typedef struct TextBuilder {
    char *bytes;
    size_t count;
    size_t capacity;
} TextBuilder;

struct UmiEditorEditEngine {
    UmiEditorTextBuffer *buffer;
    UmiEditorLineIndex *line_index;
    UmiEditorCommandTransaction *transactions;
    size_t transaction_count;
    size_t transaction_cursor;
    size_t history_capacity;
    size_t history_byte_count;
    size_t history_byte_budget;
    size_t cursor_offset;
    uint64_t command_revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int add_overflows(size_t left, size_t right)
{
    return right > SIZE_MAX - left;
}

static UmiStatus builder_reserve(TextBuilder *builder, size_t required)
{
    size_t capacity;
    char *replacement;

    if (required <= builder->capacity) return UMI_STATUS_OK;
    capacity = builder->capacity > 0U ? builder->capacity : 256U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) {
            capacity = required;
            break;
        }
        capacity *= 2U;
    }
    if (capacity == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    replacement = (char *)realloc(builder->bytes, capacity + 1U);
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    builder->bytes = replacement;
    builder->capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus builder_append(TextBuilder *builder,
                                const char *bytes,
                                size_t byte_count)
{
    UmiStatus status;

    if (builder == NULL || (bytes == NULL && byte_count > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (add_overflows(builder->count, byte_count)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = builder_reserve(builder, builder->count + byte_count);
    if (status != UMI_STATUS_OK) return status;
    if (byte_count > 0U) {
        (void)memcpy(builder->bytes + builder->count, bytes, byte_count);
    }
    builder->count += byte_count;
    builder->bytes[builder->count] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus builder_append_byte(TextBuilder *builder, char byte)
{
    return builder_append(builder, &byte, 1U);
}

static void builder_discard(TextBuilder *builder)
{
    if (builder == NULL) return;
    free(builder->bytes);
    builder->bytes = NULL;
    builder->count = 0U;
    builder->capacity = 0U;
}

static UmiStatus allocate_copy(const char *bytes,
                               size_t byte_count,
                               char **out_copy)
{
    char *copy;

    if (out_copy == NULL || (bytes == NULL && byte_count > 0U) ||
        byte_count == SIZE_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_copy = NULL;
    copy = (char *)malloc(byte_count + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (byte_count > 0U) (void)memcpy(copy, bytes, byte_count);
    copy[byte_count] = '\0';
    *out_copy = copy;
    return UMI_STATUS_OK;
}

static UmiStatus replace_alloc(const char *source,
                               size_t source_count,
                               size_t offset,
                               size_t removed_count,
                               const char *inserted,
                               size_t inserted_count,
                               char **out_bytes,
                               size_t *out_count)
{
    size_t result_count;
    char *result;

    if (out_bytes == NULL || out_count == NULL ||
        (source == NULL && source_count > 0U) ||
        (inserted == NULL && inserted_count > 0U) ||
        offset > source_count || removed_count > source_count - offset ||
        add_overflows(source_count - removed_count, inserted_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_bytes = NULL;
    *out_count = 0U;
    result_count = source_count - removed_count + inserted_count;
    if (result_count == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    result = (char *)malloc(result_count + 1U);
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (offset > 0U) (void)memcpy(result, source, offset);
    if (inserted_count > 0U) {
        (void)memcpy(result + offset, inserted, inserted_count);
    }
    if (source_count > offset + removed_count) {
        (void)memcpy(result + offset + inserted_count,
                     source + offset + removed_count,
                     source_count - offset - removed_count);
    }
    result[result_count] = '\0';
    *out_bytes = result;
    *out_count = result_count;
    return UMI_STATUS_OK;
}

static UmiStatus rebuild_index(UmiEditorEditEngine *engine)
{
    UmiEditorTextBufferView view;
    UmiStatus status = umi_editor_text_buffer_view(engine->buffer, &view);

    if (status != UMI_STATUS_OK) return status;
    return umi_editor_line_index_rebuild(engine->line_index,
                                          view.bytes,
                                          view.byte_count,
                                          view.revision);
}

static size_t content_end(const char *bytes, size_t start, size_t end)
{
    (void)start;
    if (end > 0U && bytes[end - 1U] == '\r') return end - 1U;
    return end;
}

static size_t block_end(const UmiEditorEditEngine *engine,
                        size_t line,
                        size_t byte_count)
{
    size_t start;
    size_t end;

    if (line + 1U < umi_editor_line_index_count(engine->line_index) &&
        umi_editor_line_index_line_range(engine->line_index,
                                         line + 1U,
                                         &start,
                                         &end) == UMI_STATUS_OK) {
        return start;
    }
    return byte_count;
}

static UmiStatus selected_lines(const UmiEditorEditEngine *engine,
                                size_t source_count,
                                const UmiEditorEditCommandRequest *request,
                                size_t *out_first,
                                size_t *out_last)
{
    size_t start = request->selection_start;
    size_t end = request->selection_end;
    UmiEditorLineLocation first;
    UmiEditorLineLocation last;
    UmiStatus status;

    if (start > end) {
        size_t swap = start;
        start = end;
        end = swap;
    }
    if (start == end) start = end = request->cursor_offset;
    if (start > source_count || end > source_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_line_index_locate_offset(engine->line_index,
                                                  start,
                                                  &first);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_line_index_locate_offset(engine->line_index,
                                                  end,
                                                  &last);
    if (status != UMI_STATUS_OK) return status;
    if (end > start && last.column_bytes == 0U && last.line > first.line) {
        last.line -= 1U;
    }
    *out_first = first.line;
    *out_last = last.line;
    return UMI_STATUS_OK;
}

static UmiStatus transform_case(const UmiEditorTextBufferView *view,
                                const UmiEditorEditCommandRequest *request,
                                int uppercase,
                                char **out_bytes,
                                size_t *out_count,
                                size_t *out_cursor,
                                size_t *out_lines)
{
    size_t start = request->selection_start;
    size_t end = request->selection_end;
    size_t index;
    UmiStatus status;

    if (start > end) {
        size_t swap = start;
        start = end;
        end = swap;
    }
    if (start == end || end > view->byte_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = allocate_copy(view->bytes, view->byte_count, out_bytes);
    if (status != UMI_STATUS_OK) return status;
    for (index = start; index < end; ++index) {
        unsigned char byte = (unsigned char)(*out_bytes)[index];
        (*out_bytes)[index] = (char)(uppercase ? toupper(byte) : tolower(byte));
    }
    *out_count = view->byte_count;
    *out_cursor = end;
    *out_lines = 1U;
    return UMI_STATUS_OK;
}

static UmiStatus transform_trim(const UmiEditorTextBufferView *view,
                                char **out_bytes,
                                size_t *out_count,
                                size_t *out_cursor,
                                size_t *out_lines)
{
    TextBuilder builder = {0};
    size_t offset;
    size_t line_start = 0U;
    size_t changed_lines = 0U;
    UmiStatus status;

    for (offset = 0U; offset < view->byte_count; ++offset) {
        char byte = view->bytes[offset];
        if (byte == '\r' || byte == '\n') {
            size_t before = builder.count;
            while (builder.count > line_start &&
                   (builder.bytes[builder.count - 1U] == ' ' ||
                    builder.bytes[builder.count - 1U] == '\t')) {
                builder.count -= 1U;
            }
            if (builder.count != before) changed_lines += 1U;
            status = builder_append_byte(&builder, byte);
            if (status != UMI_STATUS_OK) {
                builder_discard(&builder);
                return status;
            }
            if (byte == '\n') line_start = builder.count;
        } else {
            status = builder_append_byte(&builder, byte);
            if (status != UMI_STATUS_OK) {
                builder_discard(&builder);
                return status;
            }
        }
    }
    {
        size_t before = builder.count;
        while (builder.count > line_start &&
               (builder.bytes[builder.count - 1U] == ' ' ||
                builder.bytes[builder.count - 1U] == '\t')) {
            builder.count -= 1U;
        }
        if (builder.count != before) changed_lines += 1U;
        if (builder.bytes != NULL) builder.bytes[builder.count] = '\0';
    }
    if (builder.bytes == NULL) {
        status = allocate_copy("", 0U, &builder.bytes);
        if (status != UMI_STATUS_OK) return status;
    }
    *out_bytes = builder.bytes;
    *out_count = builder.count;
    *out_cursor = *out_count;
    *out_lines = changed_lines;
    return UMI_STATUS_OK;
}

static UmiStatus transform_line_prefixes(
    const UmiEditorEditEngine *engine,
    const UmiEditorTextBufferView *view,
    const UmiEditorEditCommandRequest *request,
    size_t first_line,
    size_t last_line,
    char **out_bytes,
    size_t *out_count,
    size_t *out_cursor,
    size_t *out_lines)
{
    const char *token = request->indent_text;
    size_t token_count = request->indent_byte_count;
    int commenting = request->kind == UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT;
    int indenting = request->kind == UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
    int outdenting = request->kind == UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES;
    int remove_comments = commenting;
    TextBuilder builder = {0};
    size_t line;
    size_t source_cursor = 0U;
    UmiStatus status;

    if (commenting) {
        token = request->line_comment;
        token_count = request->line_comment_byte_count;
        if (token == NULL || token_count == 0U) {
            token = "//";
            token_count = 2U;
        }
        for (line = first_line; line <= last_line; ++line) {
            size_t start;
            size_t end;
            size_t position;
            status = umi_editor_line_index_line_range(engine->line_index,
                                                       line,
                                                       &start,
                                                       &end);
            if (status != UMI_STATUS_OK) return status;
            end = content_end(view->bytes, start, end);
            position = start;
            while (position < end &&
                   (view->bytes[position] == ' ' ||
                    view->bytes[position] == '\t')) {
                position += 1U;
            }
            if (position == end) continue;
            if (token_count > end - position ||
                memcmp(view->bytes + position, token, token_count) != 0) {
                remove_comments = 0;
                break;
            }
        }
    } else if (token == NULL || token_count == 0U) {
        token = "    ";
        token_count = 4U;
    }

    for (line = 0U; line < umi_editor_line_index_count(engine->line_index);
         ++line) {
        size_t start;
        size_t end;
        size_t end_block;
        size_t position;
        size_t skip = 0U;

        status = umi_editor_line_index_line_range(engine->line_index,
                                                   line,
                                                   &start,
                                                   &end);
        if (status != UMI_STATUS_OK) goto failure;
        end_block = block_end(engine, line, view->byte_count);
        if (start > source_cursor) {
            status = builder_append(&builder,
                                    view->bytes + source_cursor,
                                    start - source_cursor);
            if (status != UMI_STATUS_OK) goto failure;
        }
        if (line < first_line || line > last_line) {
            status = builder_append(&builder,
                                    view->bytes + start,
                                    end_block - start);
            if (status != UMI_STATUS_OK) goto failure;
            source_cursor = end_block;
            continue;
        }

        position = start;
        while (position < end &&
               (view->bytes[position] == ' ' ||
                view->bytes[position] == '\t')) {
            position += 1U;
        }
        if (commenting) {
            status = builder_append(&builder,
                                    view->bytes + start,
                                    position - start);
            if (status != UMI_STATUS_OK) goto failure;
            if (position < content_end(view->bytes, start, end)) {
                if (remove_comments) {
                    skip = token_count;
                    if (position + skip < end &&
                        view->bytes[position + skip] == ' ') {
                        skip += 1U;
                    }
                } else {
                    status = builder_append(&builder, token, token_count);
                    if (status == UMI_STATUS_OK) {
                        status = builder_append_byte(&builder, ' ');
                    }
                    if (status != UMI_STATUS_OK) goto failure;
                }
            }
            status = builder_append(&builder,
                                    view->bytes + position + skip,
                                    end_block - position - skip);
        } else if (indenting) {
            status = builder_append(&builder, token, token_count);
            if (status == UMI_STATUS_OK) {
                status = builder_append(&builder,
                                        view->bytes + start,
                                        end_block - start);
            }
        } else if (outdenting) {
            if (token_count <= end - start &&
                memcmp(view->bytes + start, token, token_count) == 0) {
                skip = token_count;
            } else if (start < end && view->bytes[start] == '\t') {
                skip = 1U;
            } else {
                while (skip < token_count && start + skip < end &&
                       view->bytes[start + skip] == ' ') {
                    skip += 1U;
                }
            }
            status = builder_append(&builder,
                                    view->bytes + start + skip,
                                    end_block - start - skip);
        } else {
            status = UMI_STATUS_INVALID_ARGUMENT;
        }
        if (status != UMI_STATUS_OK) goto failure;
        source_cursor = end_block;
    }
    if (source_cursor < view->byte_count) {
        status = builder_append(&builder,
                                view->bytes + source_cursor,
                                view->byte_count - source_cursor);
        if (status != UMI_STATUS_OK) goto failure;
    }
    if (builder.bytes == NULL) {
        status = allocate_copy("", 0U, &builder.bytes);
        if (status != UMI_STATUS_OK) return status;
    }
    *out_bytes = builder.bytes;
    *out_count = builder.count;
    *out_cursor = request->cursor_offset;
    if (*out_cursor > *out_count) *out_cursor = *out_count;
    *out_lines = last_line - first_line + 1U;
    return UMI_STATUS_OK;

failure:
    builder_discard(&builder);
    return status;
}

static UmiStatus transform_line_swap(
    const UmiEditorEditEngine *engine,
    const UmiEditorTextBufferView *view,
    size_t first_line,
    size_t second_line,
    const UmiEditorEditCommandRequest *request,
    char **out_bytes,
    size_t *out_count,
    size_t *out_cursor,
    size_t *out_lines)
{
    size_t first_start;
    size_t first_end;
    size_t second_start;
    size_t second_end;
    size_t second_block_end;
    size_t first_content_end;
    size_t second_content_end;
    TextBuilder builder = {0};
    UmiStatus status;

    status = umi_editor_line_index_line_range(engine->line_index,
                                               first_line,
                                               &first_start,
                                               &first_end);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_line_index_line_range(engine->line_index,
                                               second_line,
                                               &second_start,
                                               &second_end);
    if (status != UMI_STATUS_OK) return status;
    first_content_end = content_end(view->bytes, first_start, first_end);
    second_content_end = content_end(view->bytes, second_start, second_end);
    second_block_end = block_end(engine, second_line, view->byte_count);

    status = builder_append(&builder, view->bytes, first_start);
    if (status == UMI_STATUS_OK) {
        status = builder_append(&builder,
                                view->bytes + second_start,
                                second_content_end - second_start);
    }
    if (status == UMI_STATUS_OK) {
        status = builder_append(&builder,
                                view->bytes + first_content_end,
                                second_start - first_content_end);
    }
    if (status == UMI_STATUS_OK) {
        status = builder_append(&builder,
                                view->bytes + first_start,
                                first_content_end - first_start);
    }
    if (status == UMI_STATUS_OK) {
        status = builder_append(&builder,
                                view->bytes + second_content_end,
                                second_block_end - second_content_end);
    }
    if (status == UMI_STATUS_OK) {
        status = builder_append(&builder,
                                view->bytes + second_block_end,
                                view->byte_count - second_block_end);
    }
    if (status != UMI_STATUS_OK) {
        builder_discard(&builder);
        return status;
    }
    *out_bytes = builder.bytes;
    *out_count = builder.count;
    if (request->cursor_offset >= second_start &&
        request->cursor_offset <= second_content_end) {
        *out_cursor = first_start + request->cursor_offset - second_start;
    } else if (request->cursor_offset >= first_start &&
               request->cursor_offset <= first_content_end) {
        *out_cursor = second_start + request->cursor_offset - first_start;
    } else {
        *out_cursor = request->cursor_offset;
    }
    if (*out_cursor > *out_count) *out_cursor = *out_count;
    *out_lines = 2U;
    return UMI_STATUS_OK;
}

static UmiStatus transform_line_command(
    const UmiEditorEditEngine *engine,
    const UmiEditorTextBufferView *view,
    const UmiEditorEditCommandRequest *request,
    char **out_bytes,
    size_t *out_count,
    size_t *out_cursor,
    size_t *out_lines)
{
    size_t first_line;
    size_t last_line;
    size_t start;
    size_t end;
    size_t end_block;
    size_t count = umi_editor_line_index_count(engine->line_index);
    UmiStatus status = selected_lines(engine,
                                      view->byte_count,
                                      request,
                                      &first_line,
                                      &last_line);

    if (status != UMI_STATUS_OK) return status;
    if (request->kind == UMI_EDITOR_EDIT_COMMAND_INDENT_LINES ||
        request->kind == UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES ||
        request->kind == UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT) {
        return transform_line_prefixes(engine,
                                       view,
                                       request,
                                       first_line,
                                       last_line,
                                       out_bytes,
                                       out_count,
                                       out_cursor,
                                       out_lines);
    }
    if (request->kind == UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP) {
        if (first_line == 0U) return UMI_STATUS_NOT_FOUND;
        return transform_line_swap(engine,
                                   view,
                                   first_line - 1U,
                                   first_line,
                                   request,
                                   out_bytes,
                                   out_count,
                                   out_cursor,
                                   out_lines);
    }
    if (request->kind == UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN ||
        request->kind == UMI_EDITOR_EDIT_COMMAND_TRANSPOSE_LINES) {
        if (first_line + 1U >= count) return UMI_STATUS_NOT_FOUND;
        return transform_line_swap(engine,
                                   view,
                                   first_line,
                                   first_line + 1U,
                                   request,
                                   out_bytes,
                                   out_count,
                                   out_cursor,
                                   out_lines);
    }

    status = umi_editor_line_index_line_range(engine->line_index,
                                               first_line,
                                               &start,
                                               &end);
    if (status != UMI_STATUS_OK) return status;
    end_block = block_end(engine, first_line, view->byte_count);
    if (request->kind == UMI_EDITOR_EDIT_COMMAND_DELETE_LINE) {
        size_t remove_start = start;
        size_t remove_end = end_block;
        if (count == 1U) {
            remove_start = 0U;
            remove_end = view->byte_count;
        } else if (first_line + 1U == count) {
            size_t previous_start;
            size_t previous_end;
            status = umi_editor_line_index_line_range(engine->line_index,
                                                       first_line - 1U,
                                                       &previous_start,
                                                       &previous_end);
            if (status != UMI_STATUS_OK) return status;
            remove_start = content_end(view->bytes,
                                       previous_start,
                                       previous_end);
        }
        status = replace_alloc(view->bytes,
                               view->byte_count,
                               remove_start,
                               remove_end - remove_start,
                               NULL,
                               0U,
                               out_bytes,
                               out_count);
        *out_cursor = remove_start;
        if (*out_cursor > *out_count) *out_cursor = *out_count;
        *out_lines = 1U;
        return status;
    }
    if (request->kind == UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE) {
        if (first_line + 1U < count) {
            status = replace_alloc(view->bytes,
                                   view->byte_count,
                                   end_block,
                                   0U,
                                   view->bytes + start,
                                   end_block - start,
                                   out_bytes,
                                   out_count);
            *out_cursor = end_block + request->cursor_offset - start;
        } else {
            TextBuilder insertion = {0};
            status = builder_append_byte(&insertion, '\n');
            if (status == UMI_STATUS_OK) {
                status = builder_append(&insertion,
                                        view->bytes + start,
                                        end - start);
            }
            if (status == UMI_STATUS_OK) {
                status = replace_alloc(view->bytes,
                                       view->byte_count,
                                       view->byte_count,
                                       0U,
                                       insertion.bytes,
                                       insertion.count,
                                       out_bytes,
                                       out_count);
            }
            builder_discard(&insertion);
            *out_cursor = start;
        }
        if (*out_cursor > *out_count) *out_cursor = *out_count;
        *out_lines = 1U;
        return status;
    }
    if (request->kind == UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT) {
        size_t next_start;
        size_t next_end;
        size_t left_end;
        size_t right_start;
        const char *separator = " ";
        size_t separator_count = 1U;

        if (first_line + 1U >= count) return UMI_STATUS_NOT_FOUND;
        status = umi_editor_line_index_line_range(engine->line_index,
                                                   first_line + 1U,
                                                   &next_start,
                                                   &next_end);
        if (status != UMI_STATUS_OK) return status;
        left_end = content_end(view->bytes, start, end);
        right_start = next_start;
        while (right_start < next_end &&
               (view->bytes[right_start] == ' ' ||
                view->bytes[right_start] == '\t')) {
            right_start += 1U;
        }
        if (left_end == start || right_start == next_end) {
            separator = NULL;
            separator_count = 0U;
        }
        status = replace_alloc(view->bytes,
                               view->byte_count,
                               left_end,
                               right_start - left_end,
                               separator,
                               separator_count,
                               out_bytes,
                               out_count);
        *out_cursor = left_end + separator_count;
        *out_lines = 2U;
        return status;
    }
    return UMI_STATUS_INVALID_ARGUMENT;
}

static void compute_delta(const char *before,
                          size_t before_count,
                          const char *after,
                          size_t after_count,
                          size_t *out_removed,
                          size_t *out_inserted)
{
    size_t prefix = 0U;
    size_t suffix = 0U;

    while (prefix < before_count && prefix < after_count &&
           before[prefix] == after[prefix]) {
        prefix += 1U;
    }
    while (suffix < before_count - prefix && suffix < after_count - prefix &&
           before[before_count - suffix - 1U] ==
               after[after_count - suffix - 1U]) {
        suffix += 1U;
    }
    *out_removed = before_count - prefix - suffix;
    *out_inserted = after_count - prefix - suffix;
}

static void free_transaction(UmiEditorCommandTransaction *transaction)
{
    if (transaction == NULL) return;
    free(transaction->before_bytes);
    free(transaction->after_bytes);
    (void)memset(transaction, 0, sizeof(*transaction));
}

static void truncate_redo(UmiEditorEditEngine *engine)
{
    size_t index;

    for (index = engine->transaction_cursor;
         index < engine->transaction_count;
         ++index) {
        engine->history_byte_count -=
            engine->transactions[index].before_count +
            engine->transactions[index].after_count;
        free_transaction(&engine->transactions[index]);
    }
    engine->transaction_count = engine->transaction_cursor;
}

static void evict_oldest(UmiEditorEditEngine *engine)
{
    if (engine->transaction_count == 0U) return;
    engine->history_byte_count -= engine->transactions[0].before_count +
                                  engine->transactions[0].after_count;
    free_transaction(&engine->transactions[0]);
    if (engine->transaction_count > 1U) {
        (void)memmove(engine->transactions,
                      engine->transactions + 1U,
                      (engine->transaction_count - 1U) *
                          sizeof(*engine->transactions));
    }
    engine->transaction_count -= 1U;
    if (engine->transaction_cursor > 0U) engine->transaction_cursor -= 1U;
    (void)memset(&engine->transactions[engine->transaction_count],
                 0,
                 sizeof(*engine->transactions));
}

static UmiStatus prepare_history(UmiEditorEditEngine *engine,
                                 size_t transaction_bytes)
{
    if (transaction_bytes > engine->history_byte_budget) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    truncate_redo(engine);
    while (engine->transaction_count >= engine->history_capacity ||
           transaction_bytes >
               engine->history_byte_budget - engine->history_byte_count) {
        evict_oldest(engine);
    }
    return UMI_STATUS_OK;
}

static UmiStatus generate_after(
    const UmiEditorEditEngine *engine,
    const UmiEditorTextBufferView *view,
    const UmiEditorEditCommandRequest *request,
    char **out_bytes,
    size_t *out_count,
    size_t *out_cursor,
    size_t *out_lines)
{
    size_t start;
    size_t end;

    *out_bytes = NULL;
    *out_count = 0U;
    *out_cursor = request->cursor_offset;
    *out_lines = 0U;
    if (request->cursor_offset > view->byte_count ||
        request->selection_start > view->byte_count ||
        request->selection_end > view->byte_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    switch (request->kind) {
        case UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT:
            if (request->text == NULL && request->text_byte_count > 0U) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            *out_cursor = request->cursor_offset + request->text_byte_count;
            *out_lines = 1U;
            return replace_alloc(view->bytes,
                                 view->byte_count,
                                 request->cursor_offset,
                                 0U,
                                 request->text,
                                 request->text_byte_count,
                                 out_bytes,
                                 out_count);
        case UMI_EDITOR_EDIT_COMMAND_REPLACE_RANGE:
            start = request->selection_start;
            end = request->selection_end;
            if (start > end) {
                size_t swap = start;
                start = end;
                end = swap;
            }
            if (request->text == NULL && request->text_byte_count > 0U) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            *out_cursor = start + request->text_byte_count;
            *out_lines = 1U;
            return replace_alloc(view->bytes,
                                 view->byte_count,
                                 start,
                                 end - start,
                                 request->text,
                                 request->text_byte_count,
                                 out_bytes,
                                 out_count);
        case UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE:
            return transform_case(view,
                                  request,
                                  1,
                                  out_bytes,
                                  out_count,
                                  out_cursor,
                                  out_lines);
        case UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE:
            return transform_case(view,
                                  request,
                                  0,
                                  out_bytes,
                                  out_count,
                                  out_cursor,
                                  out_lines);
        case UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE:
            return transform_trim(view,
                                  out_bytes,
                                  out_count,
                                  out_cursor,
                                  out_lines);
        default:
            return transform_line_command(engine,
                                          view,
                                          request,
                                          out_bytes,
                                          out_count,
                                          out_cursor,
                                          out_lines);
    }
}

UmiStatus umi_editor_edit_engine_create(
    const UmiEditorEditEngineConfig *config,
    UmiEditorEditEngine **out_engine)
{
    UmiEditorEditEngine *engine;
    size_t initial_capacity = UMI_EDITOR_TEXT_BUFFER_DEFAULT_CAPACITY;
    size_t history_capacity = UMI_EDITOR_EDIT_ENGINE_DEFAULT_HISTORY_CAPACITY;
    size_t history_budget = UMI_EDITOR_EDIT_ENGINE_DEFAULT_HISTORY_BYTE_BUDGET;
    UmiStatus status;

    if (out_engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_engine = NULL;
    if (config != NULL) {
        if (config->struct_size != (uint32_t)sizeof(*config) ||
            config->api_version != UMI_EDITOR_EDIT_ENGINE_API_VERSION) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        if (config->initial_buffer_capacity > 0U) {
            initial_capacity = config->initial_buffer_capacity;
        }
        if (config->history_capacity > 0U) {
            history_capacity = config->history_capacity;
        }
        if (config->history_byte_budget > 0U) {
            history_budget = config->history_byte_budget;
        }
    }
    if (history_capacity > SIZE_MAX / sizeof(*engine->transactions)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    engine = (UmiEditorEditEngine *)calloc(1U, sizeof(*engine));
    if (engine == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    engine->transactions = (UmiEditorCommandTransaction *)calloc(
        history_capacity, sizeof(*engine->transactions));
    if (engine->transactions == NULL) {
        free(engine);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    engine->history_capacity = history_capacity;
    engine->history_byte_budget = history_budget;
    engine->command_revision = 1U;
    status = umi_editor_text_buffer_create(initial_capacity, &engine->buffer);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_line_index_create(&engine->line_index);
    }
    if (status == UMI_STATUS_OK) status = rebuild_index(engine);
    if (status != UMI_STATUS_OK) {
        umi_editor_edit_engine_destroy(engine);
        return status;
    }
    *out_engine = engine;
    return UMI_STATUS_OK;
}

void umi_editor_edit_engine_destroy(UmiEditorEditEngine *engine)
{
    size_t index;

    if (engine == NULL) return;
    for (index = 0U; index < engine->transaction_count; ++index) {
        free_transaction(&engine->transactions[index]);
    }
    free(engine->transactions);
    umi_editor_line_index_destroy(engine->line_index);
    umi_editor_text_buffer_destroy(engine->buffer);
    free(engine);
}

UmiStatus umi_editor_edit_engine_load(UmiEditorEditEngine *engine,
                                      const char *bytes,
                                      size_t byte_count)
{
    UmiStatus status;

    if (engine == NULL || (bytes == NULL && byte_count > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_text_buffer_set(engine->buffer, bytes, byte_count);
    if (status == UMI_STATUS_OK) status = rebuild_index(engine);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_text_buffer_mark_saved(engine->buffer);
    }
    if (status == UMI_STATUS_OK) {
        (void)umi_editor_edit_engine_clear_history(engine);
        engine->cursor_offset = 0U;
        engine->command_revision = next_revision(engine->command_revision);
    }
    return status;
}

UmiStatus umi_editor_edit_engine_execute(
    UmiEditorEditEngine *engine,
    const UmiEditorEditCommandRequest *request,
    UmiEditorEditCommandResult *out_result)
{
    UmiEditorTextBufferView before_view;
    UmiEditorCommandTransaction transaction = {0};
    char *after_bytes = NULL;
    size_t after_count = 0U;
    size_t after_cursor = 0U;
    size_t affected_lines = 0U;
    size_t transaction_bytes;
    UmiStatus status;

    if (engine == NULL || request == NULL || out_result == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_EDIT_COMMAND_API_VERSION ||
        umi_editor_edit_command_for_kind(request->kind) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_text_buffer_view(engine->buffer, &before_view);
    if (status != UMI_STATUS_OK) return status;
    status = generate_after(engine,
                            &before_view,
                            request,
                            &after_bytes,
                            &after_count,
                            &after_cursor,
                            &affected_lines);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_EDITOR_EDIT_COMMAND_API_VERSION;
    out_result->kind = request->kind;
    out_result->before_revision = before_view.revision;
    out_result->after_revision = before_view.revision;
    out_result->cursor_offset = request->cursor_offset;
    out_result->selection_start = request->selection_start;
    out_result->selection_end = request->selection_end;
    out_result->affected_line_count = affected_lines;
    if (after_count == before_view.byte_count &&
        memcmp(after_bytes, before_view.bytes, after_count) == 0) {
        free(after_bytes);
        return UMI_STATUS_OK;
    }

    if (add_overflows(before_view.byte_count, after_count)) {
        free(after_bytes);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    transaction_bytes = before_view.byte_count + after_count;
    status = prepare_history(engine, transaction_bytes);
    if (status != UMI_STATUS_OK) {
        free(after_bytes);
        return status;
    }
    status = allocate_copy(before_view.bytes,
                           before_view.byte_count,
                           &transaction.before_bytes);
    if (status != UMI_STATUS_OK) {
        free(after_bytes);
        return status;
    }
    status = umi_editor_text_buffer_reserve(engine->buffer, after_count);
    if (status != UMI_STATUS_OK) {
        free(transaction.before_bytes);
        free(after_bytes);
        return status;
    }
    transaction.kind = request->kind;
    transaction.before_count = before_view.byte_count;
    transaction.after_bytes = after_bytes;
    transaction.after_count = after_count;
    transaction.before_cursor = request->cursor_offset;
    transaction.after_cursor = after_cursor;
    transaction.affected_line_count = affected_lines;
    compute_delta(transaction.before_bytes,
                  transaction.before_count,
                  transaction.after_bytes,
                  transaction.after_count,
                  &transaction.removed_byte_count,
                  &transaction.inserted_byte_count);

    status = umi_editor_text_buffer_set(engine->buffer,
                                         transaction.after_bytes,
                                         transaction.after_count);
    if (status == UMI_STATUS_OK) status = rebuild_index(engine);
    if (status != UMI_STATUS_OK) {
        free_transaction(&transaction);
        return status;
    }
    engine->transactions[engine->transaction_count++] = transaction;
    engine->transaction_cursor = engine->transaction_count;
    engine->history_byte_count += transaction_bytes;
    engine->cursor_offset = after_cursor;
    engine->command_revision = next_revision(engine->command_revision);

    out_result->after_revision = umi_editor_text_buffer_revision(engine->buffer);
    out_result->cursor_offset = after_cursor;
    out_result->selection_start = after_cursor;
    out_result->selection_end = after_cursor;
    out_result->removed_byte_count = transaction.removed_byte_count;
    out_result->inserted_byte_count = transaction.inserted_byte_count;
    out_result->changed = 1;
    return UMI_STATUS_OK;
}

static UmiStatus restore_transaction(UmiEditorEditEngine *engine,
                                     UmiEditorCommandTransaction *transaction,
                                     int redo,
                                     UmiEditorEditCommandResult *out_result)
{
    const char *bytes = redo ? transaction->after_bytes
                             : transaction->before_bytes;
    size_t byte_count = redo ? transaction->after_count
                             : transaction->before_count;
    size_t cursor = redo ? transaction->after_cursor
                         : transaction->before_cursor;
    uint64_t before_revision = umi_editor_text_buffer_revision(engine->buffer);
    UmiStatus status = umi_editor_text_buffer_set(engine->buffer,
                                                   bytes,
                                                   byte_count);

    if (status == UMI_STATUS_OK) status = rebuild_index(engine);
    if (status != UMI_STATUS_OK) return status;
    engine->cursor_offset = cursor;
    engine->command_revision = next_revision(engine->command_revision);
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_EDITOR_EDIT_COMMAND_API_VERSION;
    out_result->kind = transaction->kind;
    out_result->before_revision = before_revision;
    out_result->after_revision = umi_editor_text_buffer_revision(engine->buffer);
    out_result->cursor_offset = cursor;
    out_result->selection_start = cursor;
    out_result->selection_end = cursor;
    out_result->affected_line_count = transaction->affected_line_count;
    out_result->removed_byte_count = redo
        ? transaction->removed_byte_count
        : transaction->inserted_byte_count;
    out_result->inserted_byte_count = redo
        ? transaction->inserted_byte_count
        : transaction->removed_byte_count;
    out_result->changed = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_edit_engine_undo(
    UmiEditorEditEngine *engine,
    UmiEditorEditCommandResult *out_result)
{
    UmiStatus status;

    if (engine == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (engine->transaction_cursor == 0U) return UMI_STATUS_NOT_FOUND;
    status = restore_transaction(engine,
                                 &engine->transactions[
                                     engine->transaction_cursor - 1U],
                                 0,
                                 out_result);
    if (status == UMI_STATUS_OK) engine->transaction_cursor -= 1U;
    return status;
}

UmiStatus umi_editor_edit_engine_redo(
    UmiEditorEditEngine *engine,
    UmiEditorEditCommandResult *out_result)
{
    UmiStatus status;

    if (engine == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (engine->transaction_cursor >= engine->transaction_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = restore_transaction(engine,
                                 &engine->transactions[
                                     engine->transaction_cursor],
                                 1,
                                 out_result);
    if (status == UMI_STATUS_OK) engine->transaction_cursor += 1U;
    return status;
}

UmiStatus umi_editor_edit_engine_clear_history(UmiEditorEditEngine *engine)
{
    size_t index;

    if (engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < engine->transaction_count; ++index) {
        free_transaction(&engine->transactions[index]);
    }
    engine->transaction_count = 0U;
    engine->transaction_cursor = 0U;
    engine->history_byte_count = 0U;
    engine->command_revision = next_revision(engine->command_revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_edit_engine_mark_saved(UmiEditorEditEngine *engine)
{
    if (engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_editor_text_buffer_mark_saved(engine->buffer);
}

UmiStatus umi_editor_edit_engine_snapshot(
    const UmiEditorEditEngine *engine,
    UmiEditorEditEngineSnapshot *out_snapshot)
{
    UmiEditorTextBufferView view;
    UmiStatus status;

    if (engine == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_text_buffer_view(engine->buffer, &view);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_EDIT_ENGINE_API_VERSION;
    out_snapshot->byte_count = view.byte_count;
    out_snapshot->line_count = umi_editor_line_index_count(engine->line_index);
    out_snapshot->cursor_offset = engine->cursor_offset;
    out_snapshot->undo_count = engine->transaction_cursor;
    out_snapshot->redo_count = engine->transaction_count -
                               engine->transaction_cursor;
    out_snapshot->history_byte_count = engine->history_byte_count;
    out_snapshot->history_byte_budget = engine->history_byte_budget;
    out_snapshot->document_revision = view.revision;
    out_snapshot->command_revision = engine->command_revision;
    out_snapshot->dirty = view.dirty;
    out_snapshot->can_undo = engine->transaction_cursor > 0U;
    out_snapshot->can_redo = engine->transaction_cursor <
                             engine->transaction_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_edit_engine_view(
    const UmiEditorEditEngine *engine,
    UmiEditorTextBufferView *out_view)
{
    if (engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_editor_text_buffer_view(engine->buffer, out_view);
}

const UmiEditorLineIndex *umi_editor_edit_engine_line_index(
    const UmiEditorEditEngine *engine)
{
    return engine != NULL ? engine->line_index : NULL;
}
