/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_lexer.c
 *
 * PURPOSE:
 *   Exercise the expression lexer reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_lexer.h"
int main(void) { UmiUiReactiveExpressionLexer item; umi_ui_reactive_expression_lexer_init(&item); return umi_ui_reactive_expression_lexer_valid(&item) ? 0 : 1; }
