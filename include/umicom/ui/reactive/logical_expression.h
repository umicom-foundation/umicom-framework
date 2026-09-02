/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/logical_expression.h
 *
 * PURPOSE:
 *   Represent boolean logical operands and result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_LOGICAL_EXPRESSION_H
#define UMICOM_UI_REACTIVE_LOGICAL_EXPRESSION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive logical expression data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveLogicalExpression {
    bool left;
    bool right;
    int operation;
    bool result;
} UmiUiReactiveLogicalExpression;
/**
 * Provide the ui reactive logical expression eval operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_logical_expression_eval(UmiUiReactiveLogicalExpression *item);
#ifdef __cplusplus
}
#endif
#endif
