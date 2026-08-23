/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/selection.c
 *
 * PURPOSE:
 *   Implement one-based source line-range extraction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/selection.h"

#include <string.h>

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

    if (text == NULL || out_text == NULL ||
        capacity == 0U || out_length == NULL ||
        start_line == 0U || end_line < start_line) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = text;

    while (1) {
        if (line == start_line && begin == NULL) begin = cursor;
        if (line == end_line + 1U) {
            end = cursor;
            break;
        }

        if (*cursor == '\0') {
            end = cursor;
            break;
        }

        if (*cursor == '\n') line += 1U;
        cursor += 1;
    }

    if (begin == NULL) return UMI_STATUS_NOT_FOUND;

    length = (size_t)(end - begin);

    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (length > 0U) (void)memcpy(out_text, begin, length);
    out_text[length] = '\0';
    *out_length = length;
    return UMI_STATUS_OK;
}
