/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_token.c
 *
 * PURPOSE:
 *   Exercise the expression token reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_token.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveExpressionToken item; umi_ui_reactive_expression_token_init(&item); return umi_ui_reactive_expression_token_valid(&item) ? 0 : 1; }
