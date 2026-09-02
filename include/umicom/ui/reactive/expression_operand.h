/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_operand.h
 *
 * PURPOSE:
 *   Represent scalar expression operands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_OPERAND_H
#define UMICOM_UI_REACTIVE_EXPRESSION_OPERAND_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression operand data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionOperand {
    UmiUiValue value;
    bool resolved;
} UmiUiReactiveExpressionOperand;
/**
 * Initialise ui reactive expression operand from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_expression_operand_init(UmiUiReactiveExpressionOperand *item);
/**
 * Check that ui reactive expression operand satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_expression_operand_valid(const UmiUiReactiveExpressionOperand *item);
#ifdef __cplusplus
}
#endif
#endif
