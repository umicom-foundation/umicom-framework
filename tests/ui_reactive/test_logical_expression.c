/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_logical_expression.c
 *
 * PURPOSE:
 *   Exercise the logical expression reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/logical_expression.h"
int main(void) { UmiUiReactiveLogicalExpression x={true,false,2,false}; if(umi_ui_reactive_logical_expression_eval(&x)!=UMI_STATUS_OK)return 1;return x.result?0:2; }
