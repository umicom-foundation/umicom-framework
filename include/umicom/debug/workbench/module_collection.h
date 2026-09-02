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

/**
 * Represent the debug workbench module collection data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchModuleCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchModuleCollection;

/**
 * Initialise debug workbench module collection from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_module_collection_init(UmiDebugWorkbenchModuleCollection *model);
/**
 * Add debug workbench module collection only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_debug_workbench_module_collection_add(UmiDebugWorkbenchModuleCollection *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench module collection while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_debug_workbench_module_collection_remove(UmiDebugWorkbenchModuleCollection *model, const char *id);
/**
 * Provide the debug workbench module collection select operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_module_collection_select(UmiDebugWorkbenchModuleCollection *model, const char *id);
/**
 * Find debug workbench module collection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_module_collection_find(const UmiDebugWorkbenchModuleCollection *model, const char *id);
/**
 * Check that debug workbench module collection satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_module_collection_valid(const UmiDebugWorkbenchModuleCollection *model);

#ifdef __cplusplus
}
#endif
#endif
