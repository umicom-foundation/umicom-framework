/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/loaded_source_collection.h
 *
 * PURPOSE:
 *   Maintain loaded-source ordering and source-selection state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_LOADED_SOURCE_COLLECTION_H
#define UMICOM_DEBUG_WORKBENCH_LOADED_SOURCE_COLLECTION_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench loaded source collection data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchLoadedSourceCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchLoadedSourceCollection;

/**
 * Initialise debug workbench loaded source collection from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_loaded_source_collection_init(UmiDebugWorkbenchLoadedSourceCollection *model);
/**
 * Add debug workbench loaded source collection only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_debug_workbench_loaded_source_collection_add(UmiDebugWorkbenchLoadedSourceCollection *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench loaded source collection while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_workbench_loaded_source_collection_remove(UmiDebugWorkbenchLoadedSourceCollection *model, const char *id);
/**
 * Provide the debug workbench loaded source collection select operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_loaded_source_collection_select(UmiDebugWorkbenchLoadedSourceCollection *model, const char *id);
/**
 * Find debug workbench loaded source collection while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_loaded_source_collection_find(const UmiDebugWorkbenchLoadedSourceCollection *model, const char *id);
/**
 * Check that debug workbench loaded source collection satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_loaded_source_collection_valid(const UmiDebugWorkbenchLoadedSourceCollection *model);

#ifdef __cplusplus
}
#endif
#endif
