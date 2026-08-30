/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/memory_region_model.h
 *
 * PURPOSE:
 *   Represent one readable/writable debuggee memory region and range metadata.
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
#ifndef UMICOM_DEBUG_WORKBENCH_MEMORY_REGION_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_MEMORY_REGION_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchMemoryRegionModel {
    UmiDebugWorkbenchEntry value;
    uint64_t start_address;
    uint64_t extent;
    uint64_t cursor_address;
    bool follow_execution;
    uint64_t revision;
} UmiDebugWorkbenchMemoryRegionModel;

UmiStatus umi_debug_workbench_memory_region_model_init(UmiDebugWorkbenchMemoryRegionModel *model, const char *id, uint64_t start_address, uint64_t extent);
UmiStatus umi_debug_workbench_memory_region_model_set_cursor(UmiDebugWorkbenchMemoryRegionModel *model, uint64_t address);
UmiStatus umi_debug_workbench_memory_region_model_set_follow_execution(UmiDebugWorkbenchMemoryRegionModel *model, bool follow);
int umi_debug_workbench_memory_region_model_contains(const UmiDebugWorkbenchMemoryRegionModel *model, uint64_t address);
int umi_debug_workbench_memory_region_model_valid(const UmiDebugWorkbenchMemoryRegionModel *model);

#ifdef __cplusplus
}
#endif
#endif
