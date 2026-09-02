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

/* Provide the split lines operation used by this module and its client applications. */
static UmiStatus split_lines(const char *text,
                             MergeLine *lines,
                             size_t capacity,
                             size_t *out_count)
{
    const char *cursor;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || lines == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL
            ? (size_t)(end - cursor)
            : strlen(cursor);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U && cursor[length - 1U] == '\r') {
            length -= 1U;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count >= capacity ||
            length >= UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(lines[count].text, cursor, length);
        lines[count].text[length] = '\0';
        count += 1U;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (text[0] == '\0') {
        lines[0].text[0] = '\0';
        count = 1U;
    }

    *out_count = count;
    return UMI_STATUS_OK;
}

/* Provide the copy line operation used by this module and its client applications. */
static void copy_line(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Initialise developer merge document from caller-provided values so later operations
 * receive a known state.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (base == NULL || left == NULL || right == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    status = split_lines(
        base_text, base, UMI_DEVELOPER_MERGE_MAX_BLOCKS, &base_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto cleanup;
    status = split_lines(
        left_text, left, UMI_DEVELOPER_MERGE_MAX_BLOCKS, &left_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto cleanup;
    status = split_lines(
        right_text, right, UMI_DEVELOPER_MERGE_MAX_BLOCKS, &right_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto cleanup;

    count = base_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (left_count > count) count = left_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (right_count > count) count = right_count;

    document = (UmiDeveloperMergeDocument *)calloc(1U, sizeof(*document));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const char *base_line = index < base_count ? base[index].text : "";
        const char *left_line = index < left_count ? left[index].text : "";
        const char *right_line = index < right_count ? right[index].text : "";
        UmiDeveloperMergeBlock *block = &document->blocks[index];

        block->index = index;
        copy_line(block->base_text, sizeof(block->base_text), base_line);
        copy_line(block->left_text, sizeof(block->left_text), left_line);
        copy_line(block->right_text, sizeof(block->right_text), right_line);

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(left_line, right_line) == 0) {
            block->resolution = UMI_DEVELOPER_MERGE_USE_LEFT;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(left_line, base_line) == 0) {
            block->resolution = UMI_DEVELOPER_MERGE_USE_RIGHT;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(right_line, base_line) == 0) {
            block->resolution = UMI_DEVELOPER_MERGE_USE_LEFT;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
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

/*
 * Release or reset state held by developer merge document so the same storage can be
 * reused safely.
 */
void umi_developer_merge_document_destroy(
    UmiDeveloperMergeDocument *document)
{
    free(document);
}

/*
 * Return the number of records represented by developer merge document block without
 * changing their state.
 */
size_t umi_developer_merge_document_block_count(
    const UmiDeveloperMergeDocument *document)
{
    return document != NULL ? document->block_count : 0U;
}

/*
 * Return the number of records represented by developer merge document conflict without
 * changing their state.
 */
size_t umi_developer_merge_document_conflict_count(
    const UmiDeveloperMergeDocument *document)
{
    return document != NULL ? document->conflict_count : 0U;
}

/*
 * Find developer merge document block while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_merge_document_block_at(
    const UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeBlock *out_block)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_block == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= document->block_count) return UMI_STATUS_NOT_FOUND;

    *out_block = document->blocks[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the developer merge document resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_merge_document_resolve(
    UmiDeveloperMergeDocument *document,
    size_t index,
    UmiDeveloperMergeResolution resolution)
{
    UmiDeveloperMergeBlock *block;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL ||
        resolution < UMI_DEVELOPER_MERGE_USE_BASE ||
        resolution > UMI_DEVELOPER_MERGE_USE_BOTH) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= document->block_count) return UMI_STATUS_NOT_FOUND;

    block = &document->blocks[index];

    /* Apply this branch only when its contract condition is satisfied. */
    if (block->conflict &&
        block->resolution == UMI_DEVELOPER_MERGE_UNRESOLVED) {
        document->conflict_count -= 1U;
    }

    block->resolution = resolution;
    return UMI_STATUS_OK;
}

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(char *out_text,
                             size_t capacity,
                             size_t *used,
                             const char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*used + length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(out_text + *used, text, length);
    *used += length;
    out_text[*used] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the append line operation used by this module and its client applications. */
static UmiStatus append_line(char *out_text,
                             size_t capacity,
                             size_t *used,
                             const char *line)
{
    UmiStatus status = append_text(out_text, capacity, used, line);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return append_text(out_text, capacity, used, "\n");
}

/*
 * Provide the developer merge document render operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_merge_document_render(
    const UmiDeveloperMergeDocument *document,
    char *out_text,
    size_t capacity,
    int include_conflict_markers)
{
    size_t index;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->block_count; ++index) {
        const UmiDeveloperMergeBlock *block = &document->blocks[index];
        UmiStatus status;

        /* Select the behaviour associated with the requested command or state value. */
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
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, block->right_text);
                }
                break;
            case UMI_DEVELOPER_MERGE_UNRESOLVED:
            default:
                /* Apply this branch only when its contract condition is satisfied. */
                if (!include_conflict_markers) {
                    return UMI_STATUS_INVALID_STATE;
                }

                status = append_line(
                    out_text, capacity, &used, "<<<<<<< LEFT");
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, block->left_text);
                }
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, "=======");
                }
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, block->right_text);
                }
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    status = append_line(
                        out_text, capacity, &used, ">>>>>>> RIGHT");
                }
                break;
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
