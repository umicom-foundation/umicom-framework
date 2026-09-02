/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/thread_group.h
 *
 * PURPOSE:
 *   Maintain thread ordering and active-thread selection for a debug session.
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
#ifndef UMICOM_DEBUG_WORKBENCH_THREAD_GROUP_H
#define UMICOM_DEBUG_WORKBENCH_THREAD_GROUP_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench thread group data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchThreadGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchThreadGroup;

/**
 * Initialise debug workbench thread group from caller-provided values so later operations
 * receive a known state.
 */
void umi_debug_workbench_thread_group_init(UmiDebugWorkbenchThreadGroup *model);
/**
 * Add debug workbench thread group only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_workbench_thread_group_add(UmiDebugWorkbenchThreadGroup *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench thread group while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_workbench_thread_group_remove(UmiDebugWorkbenchThreadGroup *model, const char *id);
/**
 * Provide the debug workbench thread group select operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workbench_thread_group_select(UmiDebugWorkbenchThreadGroup *model, const char *id);
/**
 * Find debug workbench thread group while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_thread_group_find(const UmiDebugWorkbenchThreadGroup *model, const char *id);
/**
 * Check that debug workbench thread group satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_thread_group_valid(const UmiDebugWorkbenchThreadGroup *model);

#ifdef __cplusplus
}
#endif
#endif
