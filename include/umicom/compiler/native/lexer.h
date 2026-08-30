/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/lexer.h
 *
 * PURPOSE:
 *   Tokenize a practical C23 source subset with comments, literals, identifiers, keywords, numbers and punctuation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_LEXER_H
#define UMICOM_COMPILER_NATIVE_LEXER_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/source_buffer.h"
#include "umicom/compiler/native/token_stream.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeLexerReport { size_t tokens; size_t comments; size_t errors; uint32_t final_line; } UmiNativeLexerReport;
UmiStatus umi_nc_lexer_run(const UmiNativeSourceBuffer *source,UmiNativeTokenStream *out_stream,UmiNativeLexerReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
