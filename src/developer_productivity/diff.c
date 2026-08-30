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

static UmiStatus split_lines(const char *text,
                             DiffLine *lines,
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

static int line_equal(const char *left,
                      const char *right,
                      const UmiDeveloperDiffOptions *options)
{
    size_t li = 0U;
    size_t ri = 0U;
    size_t left_length;
    size_t right_length;

    if (left == NULL || right == NULL || options == NULL) return 0;

    left_length = strlen(left);
    right_length = strlen(right);

    if (options->ignore_trailing_whitespace) {
        while (left_length > 0U &&
               isspace((unsigned char)left[left_length - 1U])) {
            left_length -= 1U;
        }
        while (right_length > 0U &&
               isspace((unsigned char)right[right_length - 1U])) {
            right_length -= 1U;
        }
    }

    while (li < left_length || ri < right_length) {
        unsigned char lc;
        unsigned char rc;

        if (options->ignore_all_whitespace) {
            while (li < left_length &&
                   isspace((unsigned char)left[li])) {
                li += 1U;
            }
            while (ri < right_length &&
                   isspace((unsigned char)right[ri])) {
                ri += 1U;
            }
        }

        if (li >= left_length || ri >= right_length) {
            return li >= left_length && ri >= right_length;
        }

        lc = (unsigned char)left[li++];
        rc = (unsigned char)right[ri++];

        if (options->ignore_case) {
            lc = (unsigned char)tolower(lc);
            rc = (unsigned char)tolower(rc);
        }

        if (lc != rc) return 0;
    }

    return 1;
}

static void copy_line(char *destination,
                      size_t capacity,
                      const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;

    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static UmiStatus append_row(
    UmiDeveloperDiffDocument *document,
    UmiDeveloperDiffRowKind kind,
    size_t left_line,
    const char *left_text,
    size_t right_line,
    const char *right_text)
{
    UmiDeveloperDiffRow *row;

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

void umi_developer_diff_options_init(UmiDeveloperDiffOptions *options)
{
    if (options == NULL) return;

    (void)memset(options, 0, sizeof(*options));
    options->context_lines = 3U;
}

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

    if (left_text == NULL || right_text == NULL ||
        out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_document = NULL;

    if (active_options == NULL) {
        umi_developer_diff_options_init(&default_options);
        active_options = &default_options;
    }

    left_lines = (DiffLine *)calloc(
        UMI_DEVELOPER_DIFF_MAX_LINES, sizeof(*left_lines));
    right_lines = (DiffLine *)calloc(
        UMI_DEVELOPER_DIFF_MAX_LINES, sizeof(*right_lines));
    document = (UmiDeveloperDiffDocument *)calloc(1U, sizeof(*document));

    if (left_lines == NULL || right_lines == NULL || document == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    status = split_lines(
        left_text,
        left_lines,
        UMI_DEVELOPER_DIFF_MAX_LINES,
        &left_count);
    if (status != UMI_STATUS_OK) goto cleanup;

    status = split_lines(
        right_text,
        right_lines,
        UMI_DEVELOPER_DIFF_MAX_LINES,
        &right_count);
    if (status != UMI_STATUS_OK) goto cleanup;

    width = right_count + 1U;
    matrix = (uint16_t *)calloc(
        (left_count + 1U) * width, sizeof(*matrix));
    if (matrix == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    for (i = left_count; i > 0U; --i) {
        for (j = right_count; j > 0U; --j) {
            const size_t left_index = i - 1U;
            const size_t right_index = j - 1U;
            const size_t cell = left_index * width + right_index;

            if (line_equal(
                    left_lines[left_index].text,
                    right_lines[right_index].text,
                    active_options)) {
                matrix[cell] =
                    (uint16_t)(1U +
                        matrix[i * width + j]);
            } else {
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

    while (i < left_count || j < right_count) {
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
            if (status != UMI_STATUS_OK) goto cleanup;
            ++i;
            ++j;
        } else if (j < right_count &&
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
            if (status != UMI_STATUS_OK) goto cleanup;
            ++j;
        } else {
            status = append_row(
                document,
                UMI_DEVELOPER_DIFF_DELETE,
                i + 1U,
                left_lines[i].text,
                0U,
                "");
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

void umi_developer_diff_document_destroy(
    UmiDeveloperDiffDocument *document)
{
    free(document);
}

size_t umi_developer_diff_document_row_count(
    const UmiDeveloperDiffDocument *document)
{
    return document != NULL ? document->row_count : 0U;
}

UmiStatus umi_developer_diff_document_row_at(
    const UmiDeveloperDiffDocument *document,
    size_t index,
    UmiDeveloperDiffRow *out_row)
{
    if (document == NULL || out_row == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= document->row_count) return UMI_STATUS_NOT_FOUND;

    *out_row = document->rows[index];
    return UMI_STATUS_OK;
}

UmiDeveloperDiffStats umi_developer_diff_document_stats(
    const UmiDeveloperDiffDocument *document)
{
    UmiDeveloperDiffStats empty = {0U, 0U, 0U, 0U, 0U};
    return document != NULL ? document->stats : empty;
}
