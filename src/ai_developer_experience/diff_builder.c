/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/diff_builder.c
 *
 * PURPOSE:
 *   Implement bounded deterministic LCS-based line diff generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/diff_builder.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINES 256U
#define LINE_STORAGE_CAPACITY \
    (MAX_INPUT_LINES * UMI_AI_DEVELOPER_DIFF_TEXT_CAPACITY)

typedef struct LineTable {
    char storage[LINE_STORAGE_CAPACITY];
    const char *lines[MAX_INPUT_LINES];
    size_t lengths[MAX_INPUT_LINES];
    size_t count;
    size_t used;
    int truncated;
} LineTable;

/* Provide the split lines operation used by this module and its client applications. */
static UmiStatus split_lines(const char *text, LineTable *table)
{
    const char *cursor;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || table == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(table, 0, sizeof(*table));
    cursor = text;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length =
            end != NULL ? (size_t)(end - cursor) : strlen(cursor);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U && cursor[length - 1U] == '\r') {
            length -= 1U;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (table->count >= MAX_INPUT_LINES) {
            table->truncated = 1;
            break;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= UMI_AI_DEVELOPER_DIFF_TEXT_CAPACITY) {
            length = UMI_AI_DEVELOPER_DIFF_TEXT_CAPACITY - 1U;
            table->truncated = 1;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (table->used + length + 1U > sizeof(table->storage)) {
            table->truncated = 1;
            break;
        }

        table->lines[table->count] = &table->storage[table->used];

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U) {
            (void)memcpy(
                &table->storage[table->used],
                cursor,
                length);
        }
        table->storage[table->used + length] = '\0';
        table->lengths[table->count] = length;
        table->used += length + 1U;
        table->count += 1U;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }

    /*
     * Preserve a final empty logical line only when the entire text is empty is
     * unnecessary for review; additions/removals are represented by text lines.
     */
    return UMI_STATUS_OK;
}

/* Provide the same line operation used by this module and its client applications. */
static int same_line(
    const LineTable *left,
    size_t left_index,
    const LineTable *right,
    size_t right_index)
{
    return left->lengths[left_index] == right->lengths[right_index] &&
        memcmp(
            left->lines[left_index],
            right->lines[right_index],
            left->lengths[left_index]) == 0;
}

/* Provide the append line operation used by this module and its client applications. */
static UmiStatus append_line(
    UmiAiDeveloperFileDiff *diff,
    UmiAiDeveloperDiffLineKind kind,
    uint32_t old_line,
    uint32_t new_line,
    const char *text)
{
    UmiAiDeveloperDiffLine *line;
    size_t length;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (diff->line_count >= UMI_AI_DEVELOPER_DIFF_LINE_CAPACITY) {
        diff->truncated = 1;
        return UMI_STATUS_OK;
    }

    line = &diff->lines[diff->line_count++];
    (void)memset(line, 0, sizeof(*line));
    line->kind = kind;
    line->old_line = old_line;
    line->new_line = new_line;

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(line->text)) {
        length = sizeof(line->text) - 1U;
        diff->truncated = 1;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(line->text, text, length);
    }
    line->text[length] = '\0';

    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_AI_DEVELOPER_DIFF_ADD) {
        diff->added_count += 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_AI_DEVELOPER_DIFF_REMOVE) {
        diff->removed_count += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        diff->context_count += 1U;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer diff build operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_diff_build(
    const char *path,
    UmiAiCodingPatchOperation operation,
    const char *before_text,
    const char *after_text,
    uint64_t before_hash,
    uint64_t after_hash,
    UmiAiDeveloperFileDiff *out_diff)
{
    LineTable before;
    LineTable after;
    uint16_t *matrix;
    size_t columns;
    size_t rows;
    size_t i;
    size_t j;
    size_t path_length;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || before_text == NULL ||
        after_text == NULL || out_diff == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    path_length = strlen(path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (path_length >= sizeof(out_diff->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = split_lines(before_text, &before);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = split_lines(after_text, &after);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_diff, 0, sizeof(*out_diff));
    (void)memcpy(out_diff->path, path, path_length + 1U);
    out_diff->operation = operation;
    out_diff->before_hash = before_hash;
    out_diff->after_hash = after_hash;
    out_diff->revision = 1U;
    out_diff->truncated = before.truncated || after.truncated;

    rows = before.count + 1U;
    columns = after.count + 1U;

    matrix = (uint16_t *)calloc(rows * columns, sizeof(*matrix));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL) return UMI_STATUS_OUT_OF_MEMORY;

#define CELL(r, c) matrix[(r) * columns + (c)]

    /* Visit each bounded item once so every record receives the same rule. */
    for (i = before.count; i > 0U; --i) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = after.count; j > 0U; --j) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (same_line(&before, i - 1U, &after, j - 1U)) {
                CELL(i - 1U, j - 1U) =
                    (uint16_t)(CELL(i, j) + 1U);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                CELL(i - 1U, j - 1U) =
                    CELL(i, j - 1U) >= CELL(i - 1U, j)
                        ? CELL(i, j - 1U)
                        : CELL(i - 1U, j);
            }
        }
    }

    i = 0U;
    j = 0U;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (i < before.count || j < after.count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (i < before.count &&
            j < after.count &&
            same_line(&before, i, &after, j)) {
            (void)append_line(
                out_diff,
                UMI_AI_DEVELOPER_DIFF_CONTEXT,
                (uint32_t)(i + 1U),
                (uint32_t)(j + 1U),
                before.lines[i]);
            i += 1U;
            j += 1U;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (j < after.count &&
                   (i == before.count ||
                    CELL(i, j + 1U) >= CELL(i + 1U, j))) {
            (void)append_line(
                out_diff,
                UMI_AI_DEVELOPER_DIFF_ADD,
                0U,
                (uint32_t)(j + 1U),
                after.lines[j]);
            j += 1U;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (i < before.count) {
            (void)append_line(
                out_diff,
                UMI_AI_DEVELOPER_DIFF_REMOVE,
                (uint32_t)(i + 1U),
                0U,
                before.lines[i]);
            i += 1U;
        }
    }

#undef CELL

    free(matrix);
    return UMI_STATUS_OK;
}
