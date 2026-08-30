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
typedef struct UmiUiReactiveExpressionLexer {
    char source[UMI_UI_REACTIVE_TEXT_CAPACITY];
    size_t offset;
    size_t token_count;
} UmiUiReactiveExpressionLexer;
void umi_ui_reactive_expression_lexer_init(UmiUiReactiveExpressionLexer *item);
int umi_ui_reactive_expression_lexer_valid(const UmiUiReactiveExpressionLexer *item);
#ifdef __cplusplus
}
#endif
#endif
