/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/breakpoint_group.h
 *
 * PURPOSE:
 *   Group breakpoint entries by project, source, kind or session.
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
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_GROUP_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_GROUP_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench breakpoint group data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchBreakpointGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointGroup;

/**
 * Initialise debug workbench breakpoint group from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_breakpoint_group_init(UmiDebugWorkbenchBreakpointGroup *model);
/**
 * Add debug workbench breakpoint group only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_debug_workbench_breakpoint_group_add(UmiDebugWorkbenchBreakpointGroup *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench breakpoint group while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_debug_workbench_breakpoint_group_remove(UmiDebugWorkbenchBreakpointGroup *model, const char *id);
/**
 * Provide the debug workbench breakpoint group select operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_group_select(UmiDebugWorkbenchBreakpointGroup *model, const char *id);
/**
 * Find debug workbench breakpoint group while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_breakpoint_group_find(const UmiDebugWorkbenchBreakpointGroup *model, const char *id);
/**
 * Check that debug workbench breakpoint group satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_breakpoint_group_valid(const UmiDebugWorkbenchBreakpointGroup *model);

#ifdef __cplusplus
}
#endif
#endif
