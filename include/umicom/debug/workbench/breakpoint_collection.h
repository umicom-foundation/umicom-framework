/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/breakpoint_collection.h
 *
 * PURPOSE:
 *   Maintain the workbench breakpoint explorer collection and active selection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_COLLECTION_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_COLLECTION_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench breakpoint collection data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchBreakpointCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointCollection;

/**
 * Initialise debug workbench breakpoint collection from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_breakpoint_collection_init(UmiDebugWorkbenchBreakpointCollection *model);
/**
 * Add debug workbench breakpoint collection only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_debug_workbench_breakpoint_collection_add(UmiDebugWorkbenchBreakpointCollection *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench breakpoint collection while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_workbench_breakpoint_collection_remove(UmiDebugWorkbenchBreakpointCollection *model, const char *id);
/**
 * Provide the debug workbench breakpoint collection select operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_collection_select(UmiDebugWorkbenchBreakpointCollection *model, const char *id);
/**
 * Find debug workbench breakpoint collection while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_breakpoint_collection_find(const UmiDebugWorkbenchBreakpointCollection *model, const char *id);
/**
 * Check that debug workbench breakpoint collection satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_breakpoint_collection_valid(const UmiDebugWorkbenchBreakpointCollection *model);

#ifdef __cplusplus
}
#endif
#endif
