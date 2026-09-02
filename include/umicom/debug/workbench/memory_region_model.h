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

/**
 * Represent the debug workbench memory region model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchMemoryRegionModel {
    UmiDebugWorkbenchEntry value;
    uint64_t start_address;
    uint64_t extent;
    uint64_t cursor_address;
    bool follow_execution;
    uint64_t revision;
} UmiDebugWorkbenchMemoryRegionModel;

/**
 * Initialise debug workbench memory region model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_memory_region_model_init(UmiDebugWorkbenchMemoryRegionModel *model, const char *id, uint64_t start_address, uint64_t extent);
/**
 * Provide the debug workbench memory region model set cursor operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_memory_region_model_set_cursor(UmiDebugWorkbenchMemoryRegionModel *model, uint64_t address);
/**
 * Provide the debug workbench memory region model set follow execution operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_memory_region_model_set_follow_execution(UmiDebugWorkbenchMemoryRegionModel *model, bool follow);
/**
 * Provide the debug workbench memory region model contains operation used by this module
 * and its client applications.
 */
int umi_debug_workbench_memory_region_model_contains(const UmiDebugWorkbenchMemoryRegionModel *model, uint64_t address);
/**
 * Check that debug workbench memory region model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_memory_region_model_valid(const UmiDebugWorkbenchMemoryRegionModel *model);

#ifdef __cplusplus
}
#endif
#endif
