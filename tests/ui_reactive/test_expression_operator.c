/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_operator.c
 *
 * PURPOSE:
 *   Exercise the expression operator reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_operator.h"
int main(void) { UmiUiReactiveExpressionOperator item; umi_ui_reactive_expression_operator_init(&item); return umi_ui_reactive_expression_operator_valid(&item) ? 0 : 1; }
