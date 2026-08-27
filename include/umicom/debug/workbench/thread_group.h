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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_THREAD_GROUP_H
#define UMICOM_DEBUG_WORKBENCH_THREAD_GROUP_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchThreadGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchThreadGroup;

void umi_debug_workbench_thread_group_init(UmiDebugWorkbenchThreadGroup *model);
UmiStatus umi_debug_workbench_thread_group_add(UmiDebugWorkbenchThreadGroup *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_thread_group_remove(UmiDebugWorkbenchThreadGroup *model, const char *id);
UmiStatus umi_debug_workbench_thread_group_select(UmiDebugWorkbenchThreadGroup *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_thread_group_find(const UmiDebugWorkbenchThreadGroup *model, const char *id);
int umi_debug_workbench_thread_group_valid(const UmiDebugWorkbenchThreadGroup *model);

#ifdef __cplusplus
}
#endif
#endif
