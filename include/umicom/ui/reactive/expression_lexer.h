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
#ifdef __cplusplus
}
#endif
#endif
