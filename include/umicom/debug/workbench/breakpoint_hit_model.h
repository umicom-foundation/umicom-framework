/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/breakpoint_hit_model.h
 *
 * PURPOSE:
 *   Represent the latest breakpoint hit and hit-count presentation state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_HIT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_HIT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench breakpoint hit model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchBreakpointHitModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointHitModel;

/**
 * Initialise debug workbench breakpoint hit model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_breakpoint_hit_model_init(UmiDebugWorkbenchBreakpointHitModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench breakpoint hit model set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_hit_model_set_state(UmiDebugWorkbenchBreakpointHitModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench breakpoint hit model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_breakpoint_hit_model_set_selected(UmiDebugWorkbenchBreakpointHitModel *model, bool selected);
/**
 * Provide the debug workbench breakpoint hit model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_hit_model_set_enabled(UmiDebugWorkbenchBreakpointHitModel *model, bool enabled);
/**
 * Check that debug workbench breakpoint hit model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_breakpoint_hit_model_valid(const UmiDebugWorkbenchBreakpointHitModel *model);

#ifdef __cplusplus
}
#endif
#endif
