/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/module_collection.h
 *
 * PURPOSE:
 *   Maintain loaded module ordering and active module selection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_MODULE_COLLECTION_H
#define UMICOM_DEBUG_WORKBENCH_MODULE_COLLECTION_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchModuleCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchModuleCollection;

void umi_debug_workbench_module_collection_init(UmiDebugWorkbenchModuleCollection *model);
UmiStatus umi_debug_workbench_module_collection_add(UmiDebugWorkbenchModuleCollection *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_module_collection_remove(UmiDebugWorkbenchModuleCollection *model, const char *id);
UmiStatus umi_debug_workbench_module_collection_select(UmiDebugWorkbenchModuleCollection *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_module_collection_find(const UmiDebugWorkbenchModuleCollection *model, const char *id);
int umi_debug_workbench_module_collection_valid(const UmiDebugWorkbenchModuleCollection *model);

#ifdef __cplusplus
}
#endif
#endif
