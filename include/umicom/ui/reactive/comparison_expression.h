/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/comparison_expression.h
 *
 * PURPOSE:
 *   Represent numeric comparison operands and result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMPARISON_EXPRESSION_H
#define UMICOM_UI_REACTIVE_COMPARISON_EXPRESSION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive comparison expression data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveComparisonExpression {
    double left;
    double right;
    int comparison;
    bool result;
} UmiUiReactiveComparisonExpression;
/**
 * Provide the ui reactive comparison expression eval operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_comparison_expression_eval(UmiUiReactiveComparisonExpression *item);
#ifdef __cplusplus
}
#endif
#endif
