/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_boolean_expression.c
 *
 * PURPOSE:
 *   Exercise the boolean expression reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/boolean_expression.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveBooleanExpression item; umi_ui_reactive_boolean_expression_init(&item); return umi_ui_reactive_boolean_expression_valid(&item) ? 0 : 1; }
