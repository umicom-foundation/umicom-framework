/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_program.c
 *
 * PURPOSE:
 *   Exercise the expression program reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_program.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveExpressionProgram item; umi_ui_reactive_expression_program_init(&item); return umi_ui_reactive_expression_program_valid(&item) ? 0 : 1; }
