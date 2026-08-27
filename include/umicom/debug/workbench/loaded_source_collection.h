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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_LOADED_SOURCE_COLLECTION_H
#define UMICOM_DEBUG_WORKBENCH_LOADED_SOURCE_COLLECTION_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchLoadedSourceCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchLoadedSourceCollection;

void umi_debug_workbench_loaded_source_collection_init(UmiDebugWorkbenchLoadedSourceCollection *model);
UmiStatus umi_debug_workbench_loaded_source_collection_add(UmiDebugWorkbenchLoadedSourceCollection *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_loaded_source_collection_remove(UmiDebugWorkbenchLoadedSourceCollection *model, const char *id);
UmiStatus umi_debug_workbench_loaded_source_collection_select(UmiDebugWorkbenchLoadedSourceCollection *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_loaded_source_collection_find(const UmiDebugWorkbenchLoadedSourceCollection *model, const char *id);
int umi_debug_workbench_loaded_source_collection_valid(const UmiDebugWorkbenchLoadedSourceCollection *model);

#ifdef __cplusplus
}
#endif
#endif
