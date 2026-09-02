/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/call_stack_model.h
 *
 * PURPOSE:
 *   Maintain call-stack entries and the focused frame for the active thread.
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
#ifndef UMICOM_DEBUG_WORKBENCH_CALL_STACK_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_CALL_STACK_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench call stack model data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchCallStackModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchCallStackModel;

/**
 * Initialise debug workbench call stack model from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_call_stack_model_init(UmiDebugWorkbenchCallStackModel *model);
/**
 * Add debug workbench call stack model only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_debug_workbench_call_stack_model_add(UmiDebugWorkbenchCallStackModel *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench call stack model while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_debug_workbench_call_stack_model_remove(UmiDebugWorkbenchCallStackModel *model, const char *id);
/**
 * Provide the debug workbench call stack model select operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_call_stack_model_select(UmiDebugWorkbenchCallStackModel *model, const char *id);
/**
 * Find debug workbench call stack model while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_call_stack_model_find(const UmiDebugWorkbenchCallStackModel *model, const char *id);
/**
 * Check that debug workbench call stack model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_call_stack_model_valid(const UmiDebugWorkbenchCallStackModel *model);

#ifdef __cplusplus
}
#endif
#endif
