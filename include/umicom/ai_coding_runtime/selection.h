/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/selection.h
 *
 * PURPOSE:
 *   Extract a bounded line-range selection from source text using the
 *   one-based line coordinates carried by UmiAiCodingRequest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_SELECTION_H
#define UMICOM_AI_CODING_RUNTIME_SELECTION_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding extract selection operation used by this module and its client
 * applications.
 */
/* Select inclusive one-based lines, retaining their LF/CRLF bytes. A start
 * beyond EOF returns NOT_FOUND; an end beyond EOF selects through EOF. Empty
 * text has an empty first line and a trailing LF creates a final empty line.
 * Both APIs preserve out_text and out_length on refusal. Character buffers
 * may overlap; out_length must be separate from both buffers. Keep live
 * storage and serialize access. The legacy entry requires a terminated string. */
UmiStatus umi_ai_coding_extract_selection(
    const char *text,
    uint32_t start_line,
    uint32_t end_line,
    char *out_text,
    size_t capacity,
    size_t *out_length);

/* Read exactly text_length bytes without requiring a terminator. NULL is
 * accepted only for an empty span. Embedded NUL returns PARSE_ERROR; the
 * reserved SIZE_MAX extent returns CAPACITY_EXCEEDED before reading input.
 * The caller guarantees the readable extent and actual writable capacity.
 * Output capacity includes its terminator; selection uses LF byte boundaries
 * and does not transcode, normalize or validate Unicode encoding. */
UmiStatus UmiAiCodingExtractSelectionSpan(
    const char *text, size_t text_length,
    uint32_t start_line, uint32_t end_line,
    char *out_text, size_t capacity, size_t *out_length);

#ifdef __cplusplus
}
#endif
#endif
