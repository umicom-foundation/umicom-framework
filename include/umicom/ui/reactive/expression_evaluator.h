/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_evaluator.h
 *
 * PURPOSE:
 *   Store deterministic expression evaluation outcome.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_EVALUATOR_H
#define UMICOM_UI_REACTIVE_EXPRESSION_EVALUATOR_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression evaluator data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionEvaluator {
    UmiUiValue result;
    size_t instructions_executed;
    bool success;
} UmiUiReactiveExpressionEvaluator;
/**
 * Initialise ui reactive expression evaluator from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_expression_evaluator_init(UmiUiReactiveExpressionEvaluator *item);
/**
 * Check that ui reactive expression evaluator satisfies its contract before another
 * service relies on it.
 */
int umi_ui_reactive_expression_evaluator_valid(const UmiUiReactiveExpressionEvaluator *item);
#ifdef __cplusplus
}
#endif
#endif
