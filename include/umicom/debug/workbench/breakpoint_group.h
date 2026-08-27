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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_GROUP_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_GROUP_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchBreakpointGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointGroup;

void umi_debug_workbench_breakpoint_group_init(UmiDebugWorkbenchBreakpointGroup *model);
UmiStatus umi_debug_workbench_breakpoint_group_add(UmiDebugWorkbenchBreakpointGroup *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_breakpoint_group_remove(UmiDebugWorkbenchBreakpointGroup *model, const char *id);
UmiStatus umi_debug_workbench_breakpoint_group_select(UmiDebugWorkbenchBreakpointGroup *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_breakpoint_group_find(const UmiDebugWorkbenchBreakpointGroup *model, const char *id);
int umi_debug_workbench_breakpoint_group_valid(const UmiDebugWorkbenchBreakpointGroup *model);

#ifdef __cplusplus
}
#endif
#endif
