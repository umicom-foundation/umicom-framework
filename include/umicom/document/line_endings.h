/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/line_endings.h
 *
 * PURPOSE:
 *   Define deterministic line-ending analysis and conversion for all editors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_LINE_ENDINGS_H
#define UMICOM_DOCUMENT_LINE_ENDINGS_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDocumentLineEndingAnalysis {
    UmiDocumentLineEnding dominant;
    size_t lf_count;
    size_t crlf_count;
    size_t cr_count;
    size_t line_count;
    int mixed;
    int final_newline;
} UmiDocumentLineEndingAnalysis;

UmiStatus umi_document_line_endings_analyse(
    const char *text,
    size_t length,
    UmiDocumentLineEndingAnalysis *out_analysis);
UmiStatus umi_document_line_endings_normalise(
    const char *text,
    size_t length,
    UmiDocumentLineEnding target,
    int ensure_final_newline,
    char **out_text,
    size_t *out_length);
void umi_document_line_endings_free(char *text);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_LINE_ENDINGS_H */
