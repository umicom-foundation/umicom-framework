/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/line_index.c
 *
 * PURPOSE:
 *   Implement a compact line-start index and logarithmic offset lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/line_index.h"

#include <stdlib.h>

struct UmiEditorLineIndex {
    size_t *starts;
    size_t line_count;
    size_t capacity;
    size_t byte_count;
    uint64_t source_revision;
};

static UmiStatus reserve_lines(UmiEditorLineIndex *index, size_t required)
{
    size_t capacity;
    size_t *replacement;

    if (required <= index->capacity) return UMI_STATUS_OK;
    capacity = index->capacity > 0U ? index->capacity : 128U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (size_t *)realloc(index->starts,
                                     capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->starts = replacement;
    index->capacity = capacity;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_line_index_create(UmiEditorLineIndex **out_index)
{
    UmiEditorLineIndex *index;
    UmiStatus status;

    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;
    index = (UmiEditorLineIndex *)calloc(1U, sizeof(*index));
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = reserve_lines(index, 1U);
    if (status != UMI_STATUS_OK) {
        free(index);
        return status;
    }
    index->starts[0] = 0U;
    index->line_count = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

void umi_editor_line_index_destroy(UmiEditorLineIndex *index)
{
    if (index == NULL) return;
    free(index->starts);
    free(index);
}

UmiStatus umi_editor_line_index_rebuild(UmiEditorLineIndex *index,
                                         const char *bytes,
                                         size_t byte_count,
                                         uint64_t source_revision)
{
    size_t line_count = 1U;
    size_t offset;
    UmiStatus status;

    if (index == NULL || (bytes == NULL && byte_count > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (offset = 0U; offset < byte_count; ++offset) {
        if (bytes[offset] == '\n') line_count += 1U;
    }
    status = reserve_lines(index, line_count);
    if (status != UMI_STATUS_OK) return status;

    index->starts[0] = 0U;
    line_count = 1U;
    for (offset = 0U; offset < byte_count; ++offset) {
        if (bytes[offset] == '\n') index->starts[line_count++] = offset + 1U;
    }
    index->line_count = line_count;
    index->byte_count = byte_count;
    index->source_revision = source_revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_line_index_line_range(const UmiEditorLineIndex *index,
                                            size_t line,
                                            size_t *out_start_offset,
                                            size_t *out_end_offset)
{
    if (index == NULL || out_start_offset == NULL || out_end_offset == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (line >= index->line_count) return UMI_STATUS_NOT_FOUND;
    *out_start_offset = index->starts[line];
    *out_end_offset = line + 1U < index->line_count
        ? index->starts[line + 1U] - 1U
        : index->byte_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_line_index_locate_offset(
    const UmiEditorLineIndex *index,
    size_t offset,
    UmiEditorLineLocation *out_location)
{
    size_t low;
    size_t high;
    size_t line;
    UmiStatus status;

    if (index == NULL || out_location == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (offset > index->byte_count) return UMI_STATUS_NOT_FOUND;
    low = 0U;
    high = index->line_count;
    while (low + 1U < high) {
        size_t middle = low + (high - low) / 2U;
        if (index->starts[middle] <= offset) low = middle;
        else high = middle;
    }
    line = low;
    status = umi_editor_line_index_line_range(index,
                                               line,
                                               &out_location->line_start_offset,
                                               &out_location->line_end_offset);
    if (status != UMI_STATUS_OK) return status;
    out_location->line = line;
    out_location->column_bytes = offset - out_location->line_start_offset;
    return UMI_STATUS_OK;
}

size_t umi_editor_line_index_count(const UmiEditorLineIndex *index)
{
    return index != NULL ? index->line_count : 0U;
}

uint64_t umi_editor_line_index_source_revision(const UmiEditorLineIndex *index)
{
    return index != NULL ? index->source_revision : 0U;
}
