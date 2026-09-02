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
