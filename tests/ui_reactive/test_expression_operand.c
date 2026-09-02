/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_operand.c
 *
 * PURPOSE:
 *   Exercise the expression operand reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_operand.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveExpressionOperand item; umi_ui_reactive_expression_operand_init(&item); return umi_ui_reactive_expression_operand_valid(&item) ? 0 : 1; }
