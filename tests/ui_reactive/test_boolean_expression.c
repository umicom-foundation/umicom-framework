/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_boolean_expression.c
 *
 * PURPOSE:
 *   Exercise the boolean expression reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/boolean_expression.h"
int main(void) { UmiUiReactiveBooleanExpression item; umi_ui_reactive_boolean_expression_init(&item); return umi_ui_reactive_boolean_expression_valid(&item) ? 0 : 1; }
