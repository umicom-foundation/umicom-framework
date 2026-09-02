/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/evaluate_expression_model.h
 *
 * PURPOSE:
 *   Represent an explicit debugger REPL/evaluate request and result state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_EVALUATE_EXPRESSION_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_EVALUATE_EXPRESSION_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench evaluate expression model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchEvaluateExpressionModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchEvaluateExpressionModel;

/**
 * Initialise debug workbench evaluate expression model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_evaluate_expression_model_init(UmiDebugWorkbenchEvaluateExpressionModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench evaluate expression model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_evaluate_expression_model_set_state(UmiDebugWorkbenchEvaluateExpressionModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench evaluate expression model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_debug_workbench_evaluate_expression_model_set_selected(UmiDebugWorkbenchEvaluateExpressionModel *model, bool selected);
/**
 * Provide the debug workbench evaluate expression model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_evaluate_expression_model_set_enabled(UmiDebugWorkbenchEvaluateExpressionModel *model, bool enabled);
/**
 * Check that debug workbench evaluate expression model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_evaluate_expression_model_valid(const UmiDebugWorkbenchEvaluateExpressionModel *model);

#ifdef __cplusplus
}
#endif
#endif
