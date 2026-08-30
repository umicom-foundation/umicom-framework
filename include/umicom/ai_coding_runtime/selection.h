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

UmiStatus umi_ai_coding_extract_selection(
    const char *text,
    uint32_t start_line,
    uint32_t end_line,
    char *out_text,
    size_t capacity,
    size_t *out_length);

#ifdef __cplusplus
}
#endif
#endif
