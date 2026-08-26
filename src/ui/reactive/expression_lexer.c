/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_lexer.c
 *
 * PURPOSE:
 *   Track bounded lexical state for declarative expressions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_lexer.h"
#include <string.h>

/* Initialise the expression lexer contract to deterministic zero/default state. */
void umi_ui_reactive_expression_lexer_init(UmiUiReactiveExpressionLexer *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_lexer_valid(const UmiUiReactiveExpressionLexer *item) {
    return item != NULL;
}
