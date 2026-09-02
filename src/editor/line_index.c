/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/line_index.c
 *
 * PURPOSE:
 *   Implement a compact line-start index and logarithmic offset lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the reserve lines operation used by this module and its client applications. */
static UmiStatus reserve_lines(UmiEditorLineIndex *index, size_t required)
{
    size_t capacity;
    size_t *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= index->capacity) return UMI_STATUS_OK;
    capacity = index->capacity > 0U ? index->capacity : 128U;
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
    replacement = (size_t *)realloc(index->starts,
                                     capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->starts = replacement;
    index->capacity = capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise editor line index from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_line_index_create(UmiEditorLineIndex **out_index)
{
    UmiEditorLineIndex *index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;
    index = (UmiEditorLineIndex *)calloc(1U, sizeof(*index));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = reserve_lines(index, 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(index);
        return status;
    }
    index->starts[0] = 0U;
    index->line_count = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor line index so the same storage can be reused
 * safely.
 */
void umi_editor_line_index_destroy(UmiEditorLineIndex *index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return;
    free(index->starts);
    free(index);
}

/*
 * Provide the editor line index rebuild operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_line_index_rebuild(UmiEditorLineIndex *index,
                                         const char *bytes,
                                         size_t byte_count,
                                         uint64_t source_revision)
{
    size_t line_count = 1U;
    size_t offset;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || (bytes == NULL && byte_count > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (offset = 0U; offset < byte_count; ++offset) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (bytes[offset] == '\n') line_count += 1U;
    }
    status = reserve_lines(index, line_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    index->starts[0] = 0U;
    line_count = 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (offset = 0U; offset < byte_count; ++offset) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (bytes[offset] == '\n') index->starts[line_count++] = offset + 1U;
    }
    index->line_count = line_count;
    index->byte_count = byte_count;
    index->source_revision = source_revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor line index line range operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_line_index_line_range(const UmiEditorLineIndex *index,
                                            size_t line,
                                            size_t *out_start_offset,
                                            size_t *out_end_offset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || out_start_offset == NULL || out_end_offset == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (line >= index->line_count) return UMI_STATUS_NOT_FOUND;
    *out_start_offset = index->starts[line];
    *out_end_offset = line + 1U < index->line_count
        ? index->starts[line + 1U] - 1U
        : index->byte_count;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor line index locate offset operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_line_index_locate_offset(
    const UmiEditorLineIndex *index,
    size_t offset,
    UmiEditorLineLocation *out_location)
{
    size_t low;
    size_t high;
    size_t line;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || out_location == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (offset > index->byte_count) return UMI_STATUS_NOT_FOUND;
    low = 0U;
    high = index->line_count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (low + 1U < high) {
        size_t middle = low + (high - low) / 2U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index->starts[middle] <= offset) low = middle;
        /* Use this fallback path when the earlier condition does not apply. */
        else high = middle;
    }
    line = low;
    status = umi_editor_line_index_line_range(index,
                                               line,
                                               &out_location->line_start_offset,
                                               &out_location->line_end_offset);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_location->line = line;
    out_location->column_bytes = offset - out_location->line_start_offset;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor line index without changing their
 * state.
 */
size_t umi_editor_line_index_count(const UmiEditorLineIndex *index)
{
    return index != NULL ? index->line_count : 0U;
}

/*
 * Provide the editor line index source revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_line_index_source_revision(const UmiEditorLineIndex *index)
{
    return index != NULL ? index->source_revision : 0U;
}
