/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_operator.h
 *
 * PURPOSE:
 *   Represent supported comparison and logical operators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_OPERATOR_H
#define UMICOM_UI_REACTIVE_EXPRESSION_OPERATOR_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveExpressionOperator {
    int op;
    int precedence;
} UmiUiReactiveExpressionOperator;
void umi_ui_reactive_expression_operator_init(UmiUiReactiveExpressionOperator *item);
int umi_ui_reactive_expression_operator_valid(const UmiUiReactiveExpressionOperator *item);
#ifdef __cplusplus
}
#endif
#endif
