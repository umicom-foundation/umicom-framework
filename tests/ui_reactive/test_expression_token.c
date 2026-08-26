/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_token.c
 *
 * PURPOSE:
 *   Exercise the expression token reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_token.h"
int main(void) { UmiUiReactiveExpressionToken item; umi_ui_reactive_expression_token_init(&item); return umi_ui_reactive_expression_token_valid(&item) ? 0 : 1; }
