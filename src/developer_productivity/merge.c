/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/merge.c
 *
 * PURPOSE:
 *   Implement a bounded line-oriented three-way merge. Identical sides resolve
 *   automatically; one-sided changes win when the other side matches base;
 *   competing changes become explicit conflicts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/merge.h"

#include <stdlib.h>
#include <string.h>

typedef struct MergeLine {
    char text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
} MergeLine;

struct UmiDeveloperMergeDocument {
    UmiDeveloperMergeBlock blocks[UMI_DEVELOPER_MERGE_MAX_BLOCKS];
    size_t block_count;
    size_t conflict_count;
};

static UmiStatus split_lines(const char *text,
                             MergeLine *lines,
                             size_t capacity,
                             size_t *out_count)
{
    const char *cursor;
    size_t count = 0U;

    if (text == NULL || lines == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = text;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL
            ? (size_t)(end - cursor)
            : strlen(cursor);

        if (length > 0U && cursor[length - 1U] == '\r') {
            length -= 1U;
        }

        if (count >= capacity ||
            length >= UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(lines[count].text, cursor, length);
        lines[count].text[length] = '\0';
        count += 1U;

        if (end == NULL) break;
        cursor = end + 1;
    }

    if (text[0] == '\0') {
        lines[0].text[0] = '\0';
        count = 1U;
    }

    *out_count = count;
    return UMI_STATUS_OK;
}

static void copy_line(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

UmiStatus umi_developer_merge_document_create(
    const char *base_text,
    const char *left_text,
    const char *right_text,
    UmiDeveloperMergeDocument **out_document)
{
    MergeLine *base = NULL;
    MergeLine *left = NULL;
    MergeLine *right = NULL;
    size_t base_count = 0U;
    size_t left_count = 0U;
    size_t right_count = 0U;
    size_t count;
    size_t index;
    UmiDeveloperMergeDocument *document;
    UmiStatus status;

    if (base_text == NULL || left_text == NULL ||
        right_text == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_document = NULL;

    base = (MergeLine *)calloc(
        UMI_DEVELOPER_MERGE_MAX_BLOCKS, sizeof(*base));
    left = (MergeLine *)calloc(
        UMI_DEVELOPER_MERGE_MAX_BLOCKS, sizeof(*left));
    right = (MergeLine *)calloc(
        UMI_DEVELOPER_MERGE_MAX_BLOCKS, sizeof(*right));

    if (base == NULL || left == NULL || right == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    status = split_lines(
        base_text, base, UMI_DEVELOPER_MERGE_MAX_BLOCKS, &base_count);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = split_lines(
        left_text, left, UMI_DEVELOPER_MERGE_MAX_BLOCKS, &left_count);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = split_lines(
        right_text, right, UMI_DEVELOPER_MERGE_MAX_BLOCKS, &right_count);
    if (status != UMI_STATUS_OK) goto cleanup;

    count = base_count;
    if (left_count > count) count = left_count;
    if (right_count > count) count = right_count;

    document = (UmiDeveloperMergeDocument *)calloc(1U, sizeof(*document));
    if (document == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    for (index = 0U; index < count; ++index) {
        const char *base_line = index < base_count ? base[index].text : "";
        const char *left_line = index < left_count ? left[index].text : "";
        const char *right_line = index < right_count ? right[index].text : "";
        UmiDeveloperMergeBlock *block = &document->blocks[index];

        block->index = index;
        copy_line(block->base_text, sizeof(block->base_text), base_line);
        copy_line(block->left_text, sizeof(block->left_text), left_line);
        copy_line(block->right_text, sizeof(block->right_text), right_line);

        if (strcmp(left_line, right_line) == 0) {
            block->resolution = UMI_DEVELOPER_MERGE_USE_LEFT;
        } else if (strcmp(left_line, base_line) == 0) {
            block->resolution = UMI_DEVELOPER_MERGE_USE_RIGHT;
        } else if (strcmp(right_line, base_line) == 0) {
            block->resolution = UMI_DEVELOPER_MERGE_USE_LEFT;
        } else {
            block->conflict = 1;
            block->resolution = UMI_DEVELOPER_MERGE_UNRESOLVED;
            document->conflict_count += 1U;
        }
    }

    document->block_count = count;
    *out_document = document;
    document = NULL;
    status = UMI_STATUS_OK;

cleanup:
    free(right);
    free(left);
    free(base);
    free(document);
    return status;
}

void umi_developer_merge_document_destroy(
    UmiDeveloperMergeDocument *document)
{
    free(document);
}

size_t umi_developer_merge_document_block_count(
    const UmiDeveloperMergeDocument *document)
{
    return document != NULL ? document->block_count : 0U;
}

size_t umi_developer_merge_document_conflict_count(
    const UmiDeveloperMergeDocument *document)
{
    return document != NULL ? document->conflict_count : 0U;
}

UmiStatus umi_developer_merge_document_block_at(
    const UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeBlock *out_block)
{
    if (document == NULL || out_block == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= document->block_count) return UMI_STATUS_NOT_FOUND;

    *out_block = document->blocks[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_merge_document_resolve(
    UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeResolution resolution)
{
    UmiDeveloperMergeBlock *block;

    if (document == NULL ||
        resolution < UMI_DEVELOPER_MERGE_USE_BASE ||
        resolution > UMI_DEVELOPER_MERGE_USE_BOTH) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= document->block_count) return UMI_STATUS_NOT_FOUND;

    block = &document->blocks[index];

    if (block->conflict &&
        block->resolution == UMI_DEVELOPER_MERGE_UNRESOLVED) {
        document->conflict_count -= 1U;
    }

    block->resolution = resolution;
    return UMI_STATUS_OK;
}

static UmiStatus append_text(char *out_text,
                             size_t capacity,
                             size_t *used,
                             const char *text)
{
    size_t length;

    if (out_text == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    if (*used + length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(out_text + *used, text, length);
    *used += length;
    out_text[*used] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus append_line(char *out_text,
                             size_t capacity,
                             size_t *used,
                             const char *line)
{
    UmiStatus status = append_text(out_text, capacity, used, line);
    if (status != UMI_STATUS_OK) return status;
    return append_text(out_text, capacity, used, "\n");
}

UmiStatus umi_developer_merge_document_render(
    const UmiDeveloperMergeDocument *document,
    char *out_text,
    size_t capacity,
    int include_conflict_markers)
{
    size_t index;
    size_t used = 0U;

    if (document == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';

    for (index = 0U; index < document->block_count; ++index) {
        const UmiDeveloperMergeBlock *block = &document->blocks[index];
        UmiStatus status;

        switch (block->resolution) {
            case UMI_DEVELOPER_MERGE_USE_BASE:
                status = append_line(
                    out_text, capacity, &used, block->base_text);
                break;
            case UMI_DEVELOPER_MERGE_USE_LEFT:
                status = append_line(
                    out_text, capacity, &used, block->left_text);
                break;
            case UMI_DEVELOPER_MERGE_USE_RIGHT:
                status = append_line(
                    out_text, capacity, &used, block->right_text);
                break;
            case UMI_DEVELOPER_MERGE_USE_BOTH:
                status = append_line(
                    out_text, capacity, &used, block->left_text);
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, block->right_text);
                }
                break;
            case UMI_DEVELOPER_MERGE_UNRESOLVED:
            default:
                if (!include_conflict_markers) {
                    return UMI_STATUS_INVALID_STATE;
                }

                status = append_line(
                    out_text, capacity, &used, "<<<<<<< LEFT");
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, block->left_text);
                }
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, "=======");
                }
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, block->right_text);
                }
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, ">>>>>>> RIGHT");
                }
                break;
        }

        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
