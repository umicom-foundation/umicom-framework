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

/*
 * Provide the document line endings analyse operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_line_endings_analyse(
    const char *text,
    size_t length,
    UmiDocumentLineEndingAnalysis *out_analysis)
{
    size_t index = 0U;
    size_t kinds = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((text == NULL && length > 0U) || out_analysis == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_analysis, 0, sizeof(*out_analysis));
    out_analysis->line_count = length > 0U ? 1U : 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < length) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text[index] == '\r') {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < length && text[index + 1U] == '\n') {
                out_analysis->crlf_count += 1U;
                index += 2U;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                out_analysis->cr_count += 1U;
                index += 1U;
            }
            out_analysis->line_count += 1U;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (text[index] == '\n') {
            out_analysis->lf_count += 1U;
            out_analysis->line_count += 1U;
            index += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            index += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_analysis->lf_count > 0U) kinds += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_analysis->crlf_count > 0U) kinds += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_analysis->cr_count > 0U) kinds += 1U;
    out_analysis->mixed = kinds > 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_analysis->mixed) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_MIXED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (out_analysis->crlf_count > 0U) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_CRLF;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (out_analysis->cr_count > 0U) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_CR;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (out_analysis->lf_count > 0U) {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_LF;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_analysis->dominant = UMI_DOCUMENT_LINE_ENDING_NONE;
    }
    out_analysis->final_newline = length > 0U &&
        (text[length - 1U] == '\n' || text[length - 1U] == '\r');
    return UMI_STATUS_OK;
}

/* Provide the ending bytes operation used by this module and its client applications. */
static const char *ending_bytes(UmiDocumentLineEnding target, size_t *out_count)
{
    /* Configure the optional target only when its feature has created it. */
    if (target == UMI_DOCUMENT_LINE_ENDING_LF) {
        *out_count = 1U;
        return "\n";
    }
    /* Configure the optional target only when its feature has created it. */
    if (target == UMI_DOCUMENT_LINE_ENDING_CRLF) {
        *out_count = 2U;
        return "\r\n";
    }
    /* Configure the optional target only when its feature has created it. */
    if (target == UMI_DOCUMENT_LINE_ENDING_CR) {
        *out_count = 1U;
        return "\r";
    }
    *out_count = 0U;
    return "";
}

/*
 * Provide the document line endings normalise operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((text == NULL && length > 0U) || out_text == NULL || out_length == NULL ||
        target < UMI_DOCUMENT_LINE_ENDING_LF || target > UMI_DOCUMENT_LINE_ENDING_CR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ending = ending_bytes(target, &ending_count);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < length) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text[index] == '\r') {
            newline_count += 1U;
            index += (index + 1U < length && text[index + 1U] == '\n') ? 2U : 1U;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (text[index] == '\n') {
            newline_count += 1U;
            index += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            index += 1U;
        }
    }
    capacity = length + newline_count * ending_count + ending_count + 1U;
    result = (char *)malloc(capacity);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < length) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text[index] == '\r' || text[index] == '\n') {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (text[index] == '\r' && index + 1U < length && text[index + 1U] == '\n') {
                index += 2U;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                index += 1U;
            }
            (void)memcpy(result + output, ending, ending_count);
            output += ending_count;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            result[output++] = text[index++];
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the document line endings free operation used by this module and its client
 * applications.
 */
void umi_document_line_endings_free(char *text)
{
    free(text);
}
