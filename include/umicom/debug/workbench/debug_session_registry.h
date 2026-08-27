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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_REGISTRY_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_REGISTRY_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugSessionRegistry {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionRegistry;

void umi_debug_workbench_debug_session_registry_init(UmiDebugWorkbenchDebugSessionRegistry *model);
UmiStatus umi_debug_workbench_debug_session_registry_add(UmiDebugWorkbenchDebugSessionRegistry *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_debug_session_registry_remove(UmiDebugWorkbenchDebugSessionRegistry *model, const char *id);
UmiStatus umi_debug_workbench_debug_session_registry_select(UmiDebugWorkbenchDebugSessionRegistry *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_session_registry_find(const UmiDebugWorkbenchDebugSessionRegistry *model, const char *id);
int umi_debug_workbench_debug_session_registry_valid(const UmiDebugWorkbenchDebugSessionRegistry *model);

#ifdef __cplusplus
}
#endif
#endif
