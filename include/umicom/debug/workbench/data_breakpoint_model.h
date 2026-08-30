/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/data_breakpoint_model.h
 *
 * PURPOSE:
 *   Represent a data-access breakpoint and its persistent workbench identity.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DATA_BREAKPOINT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DATA_BREAKPOINT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDataBreakpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDataBreakpointModel;

UmiStatus umi_debug_workbench_data_breakpoint_model_init(UmiDebugWorkbenchDataBreakpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_data_breakpoint_model_set_state(UmiDebugWorkbenchDataBreakpointModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_data_breakpoint_model_set_selected(UmiDebugWorkbenchDataBreakpointModel *model, bool selected);
UmiStatus umi_debug_workbench_data_breakpoint_model_set_enabled(UmiDebugWorkbenchDataBreakpointModel *model, bool enabled);
int umi_debug_workbench_data_breakpoint_model_valid(const UmiDebugWorkbenchDataBreakpointModel *model);

#ifdef __cplusplus
}
#endif
#endif
