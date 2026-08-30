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

typedef struct UmiDebugWorkbenchEvaluateExpressionModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchEvaluateExpressionModel;

UmiStatus umi_debug_workbench_evaluate_expression_model_init(UmiDebugWorkbenchEvaluateExpressionModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_evaluate_expression_model_set_state(UmiDebugWorkbenchEvaluateExpressionModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_evaluate_expression_model_set_selected(UmiDebugWorkbenchEvaluateExpressionModel *model, bool selected);
UmiStatus umi_debug_workbench_evaluate_expression_model_set_enabled(UmiDebugWorkbenchEvaluateExpressionModel *model, bool enabled);
int umi_debug_workbench_evaluate_expression_model_valid(const UmiDebugWorkbenchEvaluateExpressionModel *model);

#ifdef __cplusplus
}
#endif
#endif
