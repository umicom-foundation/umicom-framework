/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_operator.c
 *
 * PURPOSE:
 *   Exercise the expression operator reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_operator.h"
int main(void) { UmiUiReactiveExpressionOperator item; umi_ui_reactive_expression_operator_init(&item); return umi_ui_reactive_expression_operator_valid(&item) ? 0 : 1; }
