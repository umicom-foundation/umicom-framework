/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_operand.h
 *
 * PURPOSE:
 *   Represent scalar expression operands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_OPERAND_H
#define UMICOM_UI_REACTIVE_EXPRESSION_OPERAND_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveExpressionOperand {
    UmiUiValue value;
    bool resolved;
} UmiUiReactiveExpressionOperand;
void umi_ui_reactive_expression_operand_init(UmiUiReactiveExpressionOperand *item);
int umi_ui_reactive_expression_operand_valid(const UmiUiReactiveExpressionOperand *item);
#ifdef __cplusplus
}
#endif
#endif
