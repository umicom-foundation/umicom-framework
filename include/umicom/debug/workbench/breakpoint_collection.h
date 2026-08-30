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

typedef struct UmiDebugWorkbenchBreakpointCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointCollection;

void umi_debug_workbench_breakpoint_collection_init(UmiDebugWorkbenchBreakpointCollection *model);
UmiStatus umi_debug_workbench_breakpoint_collection_add(UmiDebugWorkbenchBreakpointCollection *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_breakpoint_collection_remove(UmiDebugWorkbenchBreakpointCollection *model, const char *id);
UmiStatus umi_debug_workbench_breakpoint_collection_select(UmiDebugWorkbenchBreakpointCollection *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_breakpoint_collection_find(const UmiDebugWorkbenchBreakpointCollection *model, const char *id);
int umi_debug_workbench_breakpoint_collection_valid(const UmiDebugWorkbenchBreakpointCollection *model);

#ifdef __cplusplus
}
#endif
#endif
