/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/exception_breakpoint_model.h
 *
 * PURPOSE:
 *   Represent exception-filter breakpoint selection and presentation state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_EXCEPTION_BREAKPOINT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_EXCEPTION_BREAKPOINT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchExceptionBreakpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchExceptionBreakpointModel;

UmiStatus umi_debug_workbench_exception_breakpoint_model_init(UmiDebugWorkbenchExceptionBreakpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_state(UmiDebugWorkbenchExceptionBreakpointModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_selected(UmiDebugWorkbenchExceptionBreakpointModel *model, bool selected);
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_enabled(UmiDebugWorkbenchExceptionBreakpointModel *model, bool enabled);
int umi_debug_workbench_exception_breakpoint_model_valid(const UmiDebugWorkbenchExceptionBreakpointModel *model);

#ifdef __cplusplus
}
#endif
#endif
