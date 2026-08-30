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

typedef struct UmiDebugWorkbenchBreakpointVerificationModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointVerificationModel;

UmiStatus umi_debug_workbench_breakpoint_verification_model_init(UmiDebugWorkbenchBreakpointVerificationModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_breakpoint_verification_model_set_state(UmiDebugWorkbenchBreakpointVerificationModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_breakpoint_verification_model_set_selected(UmiDebugWorkbenchBreakpointVerificationModel *model, bool selected);
UmiStatus umi_debug_workbench_breakpoint_verification_model_set_enabled(UmiDebugWorkbenchBreakpointVerificationModel *model, bool enabled);
int umi_debug_workbench_breakpoint_verification_model_valid(const UmiDebugWorkbenchBreakpointVerificationModel *model);

#ifdef __cplusplus
}
#endif
#endif
