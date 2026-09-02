/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/breakpoint_filter_model.h
 *
 * PURPOSE:
 *   Represent breakpoint explorer filtering and enabled/disabled visibility state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_FILTER_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_FILTER_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench breakpoint filter model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchBreakpointFilterModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointFilterModel;

/**
 * Initialise debug workbench breakpoint filter model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_breakpoint_filter_model_init(UmiDebugWorkbenchBreakpointFilterModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench breakpoint filter model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_filter_model_set_state(UmiDebugWorkbenchBreakpointFilterModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench breakpoint filter model set while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_debug_workbench_breakpoint_filter_model_set_selected(UmiDebugWorkbenchBreakpointFilterModel *model, bool selected);
/**
 * Provide the debug workbench breakpoint filter model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_filter_model_set_enabled(UmiDebugWorkbenchBreakpointFilterModel *model, bool enabled);
/**
 * Check that debug workbench breakpoint filter model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_breakpoint_filter_model_valid(const UmiDebugWorkbenchBreakpointFilterModel *model);

#ifdef __cplusplus
}
#endif
#endif
