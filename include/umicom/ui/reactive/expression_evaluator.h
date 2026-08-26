/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_evaluator.h
 *
 * PURPOSE:
 *   Store deterministic expression evaluation outcome.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_EVALUATOR_H
#define UMICOM_UI_REACTIVE_EXPRESSION_EVALUATOR_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveExpressionEvaluator {
    UmiUiValue result;
    size_t instructions_executed;
    bool success;
} UmiUiReactiveExpressionEvaluator;
void umi_ui_reactive_expression_evaluator_init(UmiUiReactiveExpressionEvaluator *item);
int umi_ui_reactive_expression_evaluator_valid(const UmiUiReactiveExpressionEvaluator *item);
#ifdef __cplusplus
}
#endif
#endif
