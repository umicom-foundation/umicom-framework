/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/hover_evaluation_model.h
 *
 * PURPOSE:
 *   Represent transient source-hover evaluation independently of editor rendering.
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
#ifndef UMICOM_DEBUG_WORKBENCH_HOVER_EVALUATION_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_HOVER_EVALUATION_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench hover evaluation model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchHoverEvaluationModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchHoverEvaluationModel;

/**
 * Initialise debug workbench hover evaluation model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_hover_evaluation_model_init(UmiDebugWorkbenchHoverEvaluationModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench hover evaluation model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_hover_evaluation_model_set_state(UmiDebugWorkbenchHoverEvaluationModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench hover evaluation model set while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_debug_workbench_hover_evaluation_model_set_selected(UmiDebugWorkbenchHoverEvaluationModel *model, bool selected);
/**
 * Provide the debug workbench hover evaluation model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_hover_evaluation_model_set_enabled(UmiDebugWorkbenchHoverEvaluationModel *model, bool enabled);
/**
 * Check that debug workbench hover evaluation model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_hover_evaluation_model_valid(const UmiDebugWorkbenchHoverEvaluationModel *model);

#ifdef __cplusplus
}
#endif
#endif
