/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_lexer.h
 *
 * PURPOSE:
 *   Track bounded lexical state for declarative expressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_LEXER_H
#define UMICOM_UI_REACTIVE_EXPRESSION_LEXER_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression lexer data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionLexer {
    char source[UMI_UI_REACTIVE_TEXT_CAPACITY];
    size_t offset;
    size_t token_count;
} UmiUiReactiveExpressionLexer;
/**
 * Initialise ui reactive expression lexer from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_expression_lexer_init(UmiUiReactiveExpressionLexer *item);
/**
 * Check that ui reactive expression lexer satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_expression_lexer_valid(const UmiUiReactiveExpressionLexer *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_expression_lexer_archive_encode(const UmiUiReactiveExpressionLexer *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_expression_lexer_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveExpressionLexer *value);

#ifdef __cplusplus
}
#endif
#endif
