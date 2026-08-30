/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/variable_presentation_model.h
 *
 * PURPOSE:
 *   Represent formatting, change highlighting and value presentation preferences.
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
#ifndef UMICOM_DEBUG_WORKBENCH_VARIABLE_PRESENTATION_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_VARIABLE_PRESENTATION_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchVariablePresentationModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchVariablePresentationModel;

UmiStatus umi_debug_workbench_variable_presentation_model_init(UmiDebugWorkbenchVariablePresentationModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_variable_presentation_model_set_state(UmiDebugWorkbenchVariablePresentationModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_variable_presentation_model_set_selected(UmiDebugWorkbenchVariablePresentationModel *model, bool selected);
UmiStatus umi_debug_workbench_variable_presentation_model_set_enabled(UmiDebugWorkbenchVariablePresentationModel *model, bool enabled);
int umi_debug_workbench_variable_presentation_model_valid(const UmiDebugWorkbenchVariablePresentationModel *model);

#ifdef __cplusplus
}
#endif
#endif
