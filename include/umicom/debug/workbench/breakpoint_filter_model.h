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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_FILTER_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_FILTER_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchBreakpointFilterModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointFilterModel;

UmiStatus umi_debug_workbench_breakpoint_filter_model_init(UmiDebugWorkbenchBreakpointFilterModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_breakpoint_filter_model_set_state(UmiDebugWorkbenchBreakpointFilterModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_breakpoint_filter_model_set_selected(UmiDebugWorkbenchBreakpointFilterModel *model, bool selected);
UmiStatus umi_debug_workbench_breakpoint_filter_model_set_enabled(UmiDebugWorkbenchBreakpointFilterModel *model, bool enabled);
int umi_debug_workbench_breakpoint_filter_model_valid(const UmiDebugWorkbenchBreakpointFilterModel *model);

#ifdef __cplusplus
}
#endif
#endif
