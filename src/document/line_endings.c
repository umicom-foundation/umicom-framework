/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/line_endings.c
 *
 * PURPOSE:
 *   Analyse mixed line endings and normalise them without changing other bytes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/line_endings.h"

#include <stdlib.h>
#include <string.h>

UmiStatus umi_document_line_endings_analyse(
    const char *text,
    size_t length,
    UmiDocumentLineEndingAnalysis *out_analysis)
{
    size_t index = 0U;
    size_t kinds = 0U;
    if ((text == NULL && length > 0U) || out_analysis == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_analysis, 0, sizeof(*out_analysis));
    out_analysis->line_count = length > 0U ? 1U : 0U;
    while (index < length) {
        if (text[index] == '\r') {
            if (index + 1U < length && text[index + 1U] == '\n') {
                out_analysis->crlf_count += 1U;
                index += 2U;
            } else {
                out_analysis->cr_count += 1U;
                index += 1U;
            }
            out_analysis->line_count += 1U;
        } else if (text[index] == '\n') {
            out_analysis->lf_count += 1U;
            out_analysis->line_count += 1U;
            index += 1U;
        } else {
            index += 1U;
        }
    }
    if (out_analysis->lf_count > 0U) kinds += 1U;
    if (out_analysis->crlf_count > 0U) kinds += 1U;
    if (out_analysis->cr_count > 0U) kinds += 1U;
    out_analysis->mixed = kinds > 1U;
    if (out_analysis->mixed) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_MIXED;
    } else if (out_analysis->crlf_count > 0U) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_CRLF;
    } else if (out_analysis->cr_count > 0U) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_CR;
    } else if (out_analysis->lf_count > 0U) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_LF;
    } else {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_NONE;
    }
    out_analysis->final_newline = length > 0U &&
        (text[length - 1U] == '\n' || text[length - 1U] == '\r');
    return UMI_STATUS_OK;
}

static const char *ending_bytes(UmiDocumentLineEnding target, size_t *out_count)
{
    if (target == UMI_DOCUMENT_LINE_ENDING_LF) {
        *out_count = 1U;
        return "\n";
    }
    if (target == UMI_DOCUMENT_LINE_ENDING_CRLF) {
        *out_count = 2U;
        return "\r\n";
    }
    if (target == UMI_DOCUMENT_LINE_ENDING_CR) {
        *out_count = 1U;
        return "\r";
    }
    *out_count = 0U;
    return "";
}

UmiStatus umi_document_line_endings_normalise(
    const char *text,
    size_t length,
    UmiDocumentLineEnding target,
    int ensure_final_newline,
    char **out_text,
    size_t *out_length)
{
    const char *ending;
    size_t ending_count;
    size_t newline_count = 0U;
    size_t index = 0U;
    size_t output = 0U;
    size_t capacity;
    char *result;
    if ((text == NULL && length > 0U) || out_text == NULL || out_length == NULL ||
        target < UMI_DOCUMENT_LINE_ENDING_LF || target > UMI_DOCUMENT_LINE_ENDING_CR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ending = ending_bytes(target, &ending_count);
    while (index < length) {
        if (text[index] == '\r') {
            newline_count += 1U;
            index += (index + 1U < length && text[index + 1U] == '\n') ? 2U : 1U;
        } else if (text[index] == '\n') {
            newline_count += 1U;
            index += 1U;
        } else {
            index += 1U;
        }
    }
    capacity = length + newline_count * ending_count + ending_count + 1U;
    result = (char *)malloc(capacity);
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index = 0U;
    while (index < length) {
        if (text[index] == '\r' || text[index] == '\n') {
            if (text[index] == '\r' && index + 1U < length && text[index + 1U] == '\n') {
                index += 2U;
            } else {
                index += 1U;
            }
            (void)memcpy(result + output, ending, ending_count);
            output += ending_count;
        } else {
            result[output++] = text[index++];
        }
    }
    if (ensure_final_newline &&
        (output == 0U || (result[output - 1U] != '\n' && result[output - 1U] != '\r'))) {
        (void)memcpy(result + output, ending, ending_count);
        output += ending_count;
    }
    result[output] = '\0';
    *out_text = result;
    *out_length = output;
    return UMI_STATUS_OK;
}

void umi_document_line_endings_free(char *text)
{
    free(text);
}
