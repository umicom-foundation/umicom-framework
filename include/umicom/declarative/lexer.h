/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/lexer.h
 *
 * PURPOSE:
 *   Split one declarative source line into bounded tokens while preserving quoted text values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_LEXER_H
#define UMICOM_DECLARATIVE_LEXER_H

#include "umicom/declarative/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DECL_MAX_TOKENS_PER_LINE 16U

typedef struct UmiDeclTokenLine {
    char tokens[UMI_DECL_MAX_TOKENS_PER_LINE][UMI_DECL_TEXT_CAPACITY];
    size_t count;
} UmiDeclTokenLine;

UmiStatus umi_decl_lexer_split_line(const char *line, UmiDeclTokenLine *out_tokens);

#ifdef __cplusplus
}
#endif

#endif
