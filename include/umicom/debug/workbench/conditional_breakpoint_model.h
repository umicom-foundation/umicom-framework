/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/conditional_breakpoint_model.h
 *
 * PURPOSE:
 *   Represent condition and hit-count expressions for a breakpoint editor.
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
#ifndef UMICOM_DEBUG_WORKBENCH_CONDITIONAL_BREAKPOINT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_CONDITIONAL_BREAKPOINT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchConditionalBreakpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchConditionalBreakpointModel;

UmiStatus umi_debug_workbench_conditional_breakpoint_model_init(UmiDebugWorkbenchConditionalBreakpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_conditional_breakpoint_model_set_state(UmiDebugWorkbenchConditionalBreakpointModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_conditional_breakpoint_model_set_selected(UmiDebugWorkbenchConditionalBreakpointModel *model, bool selected);
UmiStatus umi_debug_workbench_conditional_breakpoint_model_set_enabled(UmiDebugWorkbenchConditionalBreakpointModel *model, bool enabled);
int umi_debug_workbench_conditional_breakpoint_model_valid(const UmiDebugWorkbenchConditionalBreakpointModel *model);

#ifdef __cplusplus
}
#endif
#endif
