/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/function_breakpoint_model.h
 *
 * PURPOSE:
 *   Represent a function-name breakpoint independently of the adapter implementation.
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
#ifndef UMICOM_DEBUG_WORKBENCH_FUNCTION_BREAKPOINT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_FUNCTION_BREAKPOINT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench function breakpoint model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchFunctionBreakpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchFunctionBreakpointModel;

/**
 * Initialise debug workbench function breakpoint model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_function_breakpoint_model_init(UmiDebugWorkbenchFunctionBreakpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench function breakpoint model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_function_breakpoint_model_set_state(UmiDebugWorkbenchFunctionBreakpointModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench function breakpoint model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_debug_workbench_function_breakpoint_model_set_selected(UmiDebugWorkbenchFunctionBreakpointModel *model, bool selected);
/**
 * Provide the debug workbench function breakpoint model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_function_breakpoint_model_set_enabled(UmiDebugWorkbenchFunctionBreakpointModel *model, bool enabled);
/**
 * Check that debug workbench function breakpoint model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_function_breakpoint_model_valid(const UmiDebugWorkbenchFunctionBreakpointModel *model);

#ifdef __cplusplus
}
#endif
#endif
