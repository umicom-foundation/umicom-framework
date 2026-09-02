/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diff.c
 *
 * PURPOSE:
 *   Implement bounded exact line comparison using a longest-common-subsequence
 *   matrix, then coalesce adjacent delete/insert pairs into replacement rows.
 *
 * NOTES:
 *   The bounded engine intentionally rejects very large documents rather than
 *   consuming unbounded memory. A later streaming provider can handle huge
 *   files while preserving this stable result contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diff.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct DiffLine {
    char text[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
} DiffLine;

struct UmiDeveloperDiffDocument {
    UmiDeveloperDiffRow rows[UMI_DEVELOPER_DIFF_MAX_ROWS];
    size_t row_count;
    UmiDeveloperDiffStats stats;
};

/* Provide the split lines operation used by this module and its client applications. */
static UmiStatus split_lines(const char *text,
                             DiffLine *lines,
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

/* Provide the line equal operation used by this module and its client applications. */
static int line_equal(const char *left,
                      const char *right,
                      const UmiDeveloperDiffOptions *options)
{
    size_t li = 0U;
    size_t ri = 0U;
    size_t left_length;
    size_t right_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL || options == NULL) return 0;

    left_length = strlen(left);
    right_length = strlen(right);

    /* Apply this branch only when its contract condition is satisfied. */
    if (options->ignore_trailing_whitespace) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (left_length > 0U &&
               isspace((unsigned char)left[left_length - 1U])) {
            left_length -= 1U;
        }
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (right_length > 0U &&
               isspace((unsigned char)right[right_length - 1U])) {
            right_length -= 1U;
        }
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (li < left_length || ri < right_length) {
        unsigned char lc;
        unsigned char rc;

        /* Apply this branch only when its contract condition is satisfied. */
        if (options->ignore_all_whitespace) {
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (li < left_length &&
                   isspace((unsigned char)left[li])) {
                li += 1U;
            }
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (ri < right_length &&
                   isspace((unsigned char)right[ri])) {
                ri += 1U;
            }
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (li >= left_length || ri >= right_length) {
            return li >= left_length && ri >= right_length;
        }

        lc = (unsigned char)left[li++];
        rc = (unsigned char)right[ri++];

        /* Apply this branch only when its contract condition is satisfied. */
        if (options->ignore_case) {
            lc = (unsigned char)tolower(lc);
            rc = (unsigned char)tolower(rc);
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (lc != rc) return 0;
    }

    return 1;
}

/* Provide the copy line operation used by this module and its client applications. */
static void copy_line(char *destination,
                      size_t capacity,
                      const char *source)
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

/* Provide the append row operation used by this module and its client applications. */
static UmiStatus append_row(
    UmiDeveloperDiffDocument *document,
    UmiDeveloperDiffRowKind kind,
    size_t left_line,
    const char *left_text,
    size_t right_line,
    const char *right_text)
{
    UmiDeveloperDiffRow *row;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL ||
        document->row_count >= UMI_DEVELOPER_DIFF_MAX_ROWS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    row = &document->rows[document->row_count++];
    (void)memset(row, 0, sizeof(*row));
    row->kind = kind;
    row->left_line = left_line;
    row->right_line = right_line;
    copy_line(row->left_text, sizeof(row->left_text), left_text);
    copy_line(row->right_text, sizeof(row->right_text), right_text);

    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_DEVELOPER_DIFF_EQUAL:
            document->stats.equal_rows += 1U;
            break;
        case UMI_DEVELOPER_DIFF_INSERT:
            document->stats.inserted_rows += 1U;
            document->stats.changed_rows += 1U;
            break;
        case UMI_DEVELOPER_DIFF_DELETE:
            document->stats.deleted_rows += 1U;
            document->stats.changed_rows += 1U;
            break;
        case UMI_DEVELOPER_DIFF_REPLACE:
            document->stats.replaced_rows += 1U;
            document->stats.changed_rows += 1U;
            break;
        default:
            break;
    }

    return UMI_STATUS_OK;
}

/*
 * Initialise developer diff options from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_diff_options_init(UmiDeveloperDiffOptions *options)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options == NULL) return;

    (void)memset(options, 0, sizeof(*options));
    options->context_lines = 3U;
}

/*
 * Initialise developer diff document from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_diff_document_create(
    const char *left_text,
    const char *right_text,
    const UmiDeveloperDiffOptions *options,
    UmiDeveloperDiffDocument **out_document)
{
    DiffLine *left_lines = NULL;
    DiffLine *right_lines = NULL;
    uint16_t *matrix = NULL;
    UmiDeveloperDiffDocument *document = NULL;
    UmiDeveloperDiffOptions default_options;
    const UmiDeveloperDiffOptions *active_options = options;
    size_t left_count = 0U;
    size_t right_count = 0U;
    size_t width;
    size_t i;
    size_t j;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left_text == NULL || right_text == NULL ||
        out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_document = NULL;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (active_options == NULL) {
        umi_developer_diff_options_init(&default_options);
        active_options = &default_options;
    }

    left_lines = (DiffLine *)calloc(
        UMI_DEVELOPER_DIFF_MAX_LINES, sizeof(*left_lines));
    right_lines = (DiffLine *)calloc(
        UMI_DEVELOPER_DIFF_MAX_LINES, sizeof(*right_lines));
    document = (UmiDeveloperDiffDocument *)calloc(1U, sizeof(*document));

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left_lines == NULL || right_lines == NULL || document == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    status = split_lines(
        left_text,
        left_lines,
        UMI_DEVELOPER_DIFF_MAX_LINES,
        &left_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto cleanup;

    status = split_lines(
        right_text,
        right_lines,
        UMI_DEVELOPER_DIFF_MAX_LINES,
        &right_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto cleanup;

    width = right_count + 1U;
    matrix = (uint16_t *)calloc(
        (left_count + 1U) * width, sizeof(*matrix));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (i = left_count; i > 0U; --i) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = right_count; j > 0U; --j) {
            const size_t left_index = i - 1U;
            const size_t right_index = j - 1U;
            const size_t cell = left_index * width + right_index;

            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (line_equal(
                    left_lines[left_index].text,
                    right_lines[right_index].text,
                    active_options)) {
                matrix[cell] =
                    (uint16_t)(1U +
                        matrix[i * width + j]);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                const uint16_t down =
                    matrix[i * width + right_index];
                const uint16_t across =
                    matrix[left_index * width + j];
                matrix[cell] = down >= across ? down : across;
            }
        }
    }

    i = 0U;
    j = 0U;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (i < left_count || j < right_count) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (i < left_count && j < right_count &&
            line_equal(
                left_lines[i].text,
                right_lines[j].text,
                active_options)) {
            status = append_row(
                document,
                UMI_DEVELOPER_DIFF_EQUAL,
                i + 1U,
                left_lines[i].text,
                j + 1U,
                right_lines[j].text);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) goto cleanup;
            ++i;
            ++j;
            continue;
        }

        /*
         * If both directions have the same LCS score, present the pair as a
         * replacement. This produces the side-by-side behaviour expected by
         * professional compare tools instead of separate delete/insert rows.
         */
        if (i < left_count && j < right_count &&
            matrix[(i + 1U) * width + j] ==
                matrix[i * width + (j + 1U)]) {
            status = append_row(
                document,
                UMI_DEVELOPER_DIFF_REPLACE,
                i + 1U,
                left_lines[i].text,
                j + 1U,
                right_lines[j].text);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) goto cleanup;
            ++i;
            ++j;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (j < right_count &&
                   (i >= left_count ||
                    matrix[i * width + (j + 1U)] >
                        matrix[(i + 1U) * width + j])) {
            status = append_row(
                document,
                UMI_DEVELOPER_DIFF_INSERT,
                0U,
                "",
                j + 1U,
                right_lines[j].text);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) goto cleanup;
            ++j;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = append_row(
                document,
                UMI_DEVELOPER_DIFF_DELETE,
                i + 1U,
                left_lines[i].text,
                0U,
                "");
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) goto cleanup;
            ++i;
        }
    }

    *out_document = document;
    document = NULL;
    status = UMI_STATUS_OK;

cleanup:
    free(matrix);
    free(right_lines);
    free(left_lines);
    free(document);
    return status;
}

/*
 * Release or reset state held by developer diff document so the same storage can be reused
 * safely.
 */
void umi_developer_diff_document_destroy(
    UmiDeveloperDiffDocument *document)
{
    free(document);
}

/*
 * Return the number of records represented by developer diff document row without changing
 * their state.
 */
size_t umi_developer_diff_document_row_count(
    const UmiDeveloperDiffDocument *document)
{
    return document != NULL ? document->row_count : 0U;
}

/*
 * Find developer diff document row while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_developer_diff_document_row_at(
    const UmiDeveloperDiffDocument *document,
    size_t index,
    UmiDeveloperDiffRow *out_row)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_row == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= document->row_count) return UMI_STATUS_NOT_FOUND;

    *out_row = document->rows[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the developer diff document stats operation used by this module and its client
 * applications.
 */
UmiDeveloperDiffStats umi_developer_diff_document_stats(
    const UmiDeveloperDiffDocument *document)
{
    UmiDeveloperDiffStats empty = {0U, 0U, 0U, 0U, 0U};
    return document != NULL ? document->stats : empty;
}
