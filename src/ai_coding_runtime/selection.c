/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/selection.c
 *
 * PURPOSE:
 *   Implement one-based source line-range extraction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/selection.h"

#include <string.h>

/*
 * Provide the ai coding extract selection operation used by this module and its client
 * applications.
 */
/* Use an explicit input extent and overlap-safe publication after all reads; preserve the terminated-string entry point and established line semantics. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_extract_selection(
    const char *text,
    uint32_t start_line,
    uint32_t end_line,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    const char *cursor;
    const char *begin = NULL;
    const char *end = NULL;
    uint32_t line = 1U;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_text == NULL ||
        capacity == 0U || out_length == NULL ||
        start_line == 0U || end_line < start_line) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = text;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (1) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (line == start_line && begin == NULL) begin = cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (line == end_line + 1U) {
            end = cursor;
            break;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '\0') {
            end = cursor;
            break;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '\n') line += 1U;
        cursor += 1;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (begin == NULL) return UMI_STATUS_NOT_FOUND;

    length = (size_t)(end - begin);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(out_text, begin, length);
    out_text[length] = '\0';
    *out_length = length;
    return UMI_STATUS_OK;
}

#endif
UmiStatus UmiAiCodingExtractSelectionSpan(
    const char *text,
    size_t text_length,
    uint32_t start_line,
    uint32_t end_line,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    size_t begin = SIZE_MAX, end = 0U, length;
    uint64_t line = 1U;
    if ((text == NULL && text_length != 0U) || out_text == NULL ||
        capacity == 0U || out_length == NULL || start_line == 0U || end_line < start_line)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Reserve the sentinel and avoid offset overflow before reading a span. */
    if (text_length == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (text_length != 0U && memchr(text, '\0', text_length) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t offset = 0U;; ++offset) {
        if (line == start_line && begin == SIZE_MAX) begin = offset;
        if (line > (uint64_t)end_line || offset == text_length) {
            end = offset;
            break;
        }
        if (text[offset] == '\n') ++line;
    }
    if (begin == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    length = end - begin;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* All input inspection precedes publication, so character buffers may
     * overlap in either direction. out_length remains separate caller storage. */
    if (length != 0U) memmove(out_text, text + begin, length);
    out_text[length] = '\0';
    *out_length = length;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_extract_selection(
    const char *text,
    uint32_t start_line,
    uint32_t end_line,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    if (text == NULL || out_text == NULL || capacity == 0U ||
        out_length == NULL || start_line == 0U || end_line < start_line)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiCodingExtractSelectionSpan(text, strlen(text),
        start_line, end_line, out_text, capacity, out_length);
}
