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

/**
 * Represent the debug workbench data breakpoint model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDataBreakpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDataBreakpointModel;

/**
 * Initialise debug workbench data breakpoint model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_data_breakpoint_model_init(UmiDebugWorkbenchDataBreakpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench data breakpoint model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_data_breakpoint_model_set_state(UmiDebugWorkbenchDataBreakpointModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench data breakpoint model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_data_breakpoint_model_set_selected(UmiDebugWorkbenchDataBreakpointModel *model, bool selected);
/**
 * Provide the debug workbench data breakpoint model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_data_breakpoint_model_set_enabled(UmiDebugWorkbenchDataBreakpointModel *model, bool enabled);
/**
 * Check that debug workbench data breakpoint model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_data_breakpoint_model_valid(const UmiDebugWorkbenchDataBreakpointModel *model);

#ifdef __cplusplus
}
#endif
#endif
