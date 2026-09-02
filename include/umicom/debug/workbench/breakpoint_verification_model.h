/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/breakpoint_verification_model.h
 *
 * PURPOSE:
 *   Track adapter verification state and source binding diagnostics.
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
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_VERIFICATION_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_VERIFICATION_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench breakpoint verification model data shared with callers of
 * this public contract.
 */
typedef struct UmiDebugWorkbenchBreakpointVerificationModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointVerificationModel;

/**
 * Initialise debug workbench breakpoint verification model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_breakpoint_verification_model_init(UmiDebugWorkbenchBreakpointVerificationModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench breakpoint verification model set state operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_verification_model_set_state(UmiDebugWorkbenchBreakpointVerificationModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench breakpoint verification model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_debug_workbench_breakpoint_verification_model_set_selected(UmiDebugWorkbenchBreakpointVerificationModel *model, bool selected);
/**
 * Provide the debug workbench breakpoint verification model set enabled operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_verification_model_set_enabled(UmiDebugWorkbenchBreakpointVerificationModel *model, bool enabled);
/**
 * Check that debug workbench breakpoint verification model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_breakpoint_verification_model_valid(const UmiDebugWorkbenchBreakpointVerificationModel *model);

#ifdef __cplusplus
}
#endif
#endif
