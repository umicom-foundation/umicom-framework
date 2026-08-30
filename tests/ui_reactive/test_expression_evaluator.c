/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_evaluator.c
 *
 * PURPOSE:
 *   Exercise the expression evaluator reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_evaluator.h"
int main(void) { UmiUiReactiveExpressionEvaluator item; umi_ui_reactive_expression_evaluator_init(&item); return umi_ui_reactive_expression_evaluator_valid(&item) ? 0 : 1; }
