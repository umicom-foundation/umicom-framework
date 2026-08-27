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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_HOVER_EVALUATION_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_HOVER_EVALUATION_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchHoverEvaluationModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchHoverEvaluationModel;

UmiStatus umi_debug_workbench_hover_evaluation_model_init(UmiDebugWorkbenchHoverEvaluationModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_hover_evaluation_model_set_state(UmiDebugWorkbenchHoverEvaluationModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_hover_evaluation_model_set_selected(UmiDebugWorkbenchHoverEvaluationModel *model, bool selected);
UmiStatus umi_debug_workbench_hover_evaluation_model_set_enabled(UmiDebugWorkbenchHoverEvaluationModel *model, bool enabled);
int umi_debug_workbench_hover_evaluation_model_valid(const UmiDebugWorkbenchHoverEvaluationModel *model);

#ifdef __cplusplus
}
#endif
#endif
