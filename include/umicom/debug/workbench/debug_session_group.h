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

typedef struct UmiDebugWorkbenchDebugSessionGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionGroup;

void umi_debug_workbench_debug_session_group_init(UmiDebugWorkbenchDebugSessionGroup *model);
UmiStatus umi_debug_workbench_debug_session_group_add(UmiDebugWorkbenchDebugSessionGroup *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_debug_session_group_remove(UmiDebugWorkbenchDebugSessionGroup *model, const char *id);
UmiStatus umi_debug_workbench_debug_session_group_select(UmiDebugWorkbenchDebugSessionGroup *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_session_group_find(const UmiDebugWorkbenchDebugSessionGroup *model, const char *id);
int umi_debug_workbench_debug_session_group_valid(const UmiDebugWorkbenchDebugSessionGroup *model);

#ifdef __cplusplus
}
#endif
#endif
