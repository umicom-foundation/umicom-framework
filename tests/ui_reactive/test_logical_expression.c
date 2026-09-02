/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_logical_expression.c
 *
 * PURPOSE:
 *   Exercise the logical expression reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/logical_expression.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveLogicalExpression x={true,false,2,false}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_logical_expression_eval(&x)!=UMI_STATUS_OK)return 1;return x.result?0:2; }
