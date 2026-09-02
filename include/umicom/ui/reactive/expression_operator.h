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
/**
 * Represent the ui reactive expression operator data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionOperator {
    int op;
    int precedence;
} UmiUiReactiveExpressionOperator;
/**
 * Initialise ui reactive expression operator from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_expression_operator_init(UmiUiReactiveExpressionOperator *item);
/**
 * Check that ui reactive expression operator satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_expression_operator_valid(const UmiUiReactiveExpressionOperator *item);
#ifdef __cplusplus
}
#endif
#endif
