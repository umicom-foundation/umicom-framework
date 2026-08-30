/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/breakpoint_status_model.h
 *
 * PURPOSE:
 *   Represent aggregate breakpoint verification and enablement status.
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
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_STATUS_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_STATUS_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchBreakpointStatusModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointStatusModel;

UmiStatus umi_debug_workbench_breakpoint_status_model_init(UmiDebugWorkbenchBreakpointStatusModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_breakpoint_status_model_set_state(UmiDebugWorkbenchBreakpointStatusModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_breakpoint_status_model_set_selected(UmiDebugWorkbenchBreakpointStatusModel *model, bool selected);
UmiStatus umi_debug_workbench_breakpoint_status_model_set_enabled(UmiDebugWorkbenchBreakpointStatusModel *model, bool enabled);
int umi_debug_workbench_breakpoint_status_model_valid(const UmiDebugWorkbenchBreakpointStatusModel *model);

#ifdef __cplusplus
}
#endif
#endif
