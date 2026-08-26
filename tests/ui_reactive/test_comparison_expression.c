/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_comparison_expression.c
 *
 * PURPOSE:
 *   Exercise the comparison expression reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/comparison_expression.h"
int main(void) { UmiUiReactiveComparisonExpression x={3.0,2.0,1,false}; if(umi_ui_reactive_comparison_expression_eval(&x)!=UMI_STATUS_OK)return 1;return x.result?0:2; }
