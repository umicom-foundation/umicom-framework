/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_session_group.h
 *
 * PURPOSE:
 *   Group compound or related debug sessions under one workbench operation.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_GROUP_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_GROUP_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug session group data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugSessionGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionGroup;

/**
 * Initialise debug workbench debug session group from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_session_group_init(UmiDebugWorkbenchDebugSessionGroup *model);
/**
 * Add debug workbench debug session group only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_debug_workbench_debug_session_group_add(UmiDebugWorkbenchDebugSessionGroup *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench debug session group while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_workbench_debug_session_group_remove(UmiDebugWorkbenchDebugSessionGroup *model, const char *id);
/**
 * Provide the debug workbench debug session group select operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_group_select(UmiDebugWorkbenchDebugSessionGroup *model, const char *id);
/**
 * Find debug workbench debug session group while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_session_group_find(const UmiDebugWorkbenchDebugSessionGroup *model, const char *id);
/**
 * Check that debug workbench debug session group satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_session_group_valid(const UmiDebugWorkbenchDebugSessionGroup *model);

#ifdef __cplusplus
}
#endif
#endif
