/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_session_registry.h
 *
 * PURPOSE:
 *   Maintain multiple concurrent debug sessions and the active-session selection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_REGISTRY_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_REGISTRY_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug session registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugSessionRegistry {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionRegistry;

/**
 * Initialise debug workbench debug session registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_session_registry_init(UmiDebugWorkbenchDebugSessionRegistry *model);
/**
 * Add debug workbench debug session registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_debug_workbench_debug_session_registry_add(UmiDebugWorkbenchDebugSessionRegistry *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench debug session registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_workbench_debug_session_registry_remove(UmiDebugWorkbenchDebugSessionRegistry *model, const char *id);
/**
 * Provide the debug workbench debug session registry select operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_registry_select(UmiDebugWorkbenchDebugSessionRegistry *model, const char *id);
/**
 * Find debug workbench debug session registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_session_registry_find(const UmiDebugWorkbenchDebugSessionRegistry *model, const char *id);
/**
 * Check that debug workbench debug session registry satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_session_registry_valid(const UmiDebugWorkbenchDebugSessionRegistry *model);

#ifdef __cplusplus
}
#endif
#endif
