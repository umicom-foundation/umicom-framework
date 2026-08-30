/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_lexer.c
 *
 * PURPOSE:
 *   Exercise the expression lexer reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_lexer.h"
int main(void) { UmiUiReactiveExpressionLexer item; umi_ui_reactive_expression_lexer_init(&item); return umi_ui_reactive_expression_lexer_valid(&item) ? 0 : 1; }
