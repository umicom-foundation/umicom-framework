/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/memory_viewport_model.h
 *
 * PURPOSE:
 *   Track toolkit-neutral memory viewport address, extent and bytes-per-row state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_MEMORY_VIEWPORT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_MEMORY_VIEWPORT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchMemoryViewportModel {
    UmiDebugWorkbenchEntry value;
    uint64_t start_address;
    uint64_t extent;
    uint64_t cursor_address;
    bool follow_execution;
    uint64_t revision;
} UmiDebugWorkbenchMemoryViewportModel;

UmiStatus umi_debug_workbench_memory_viewport_model_init(UmiDebugWorkbenchMemoryViewportModel *model, const char *id, uint64_t start_address, uint64_t extent);
UmiStatus umi_debug_workbench_memory_viewport_model_set_cursor(UmiDebugWorkbenchMemoryViewportModel *model, uint64_t address);
UmiStatus umi_debug_workbench_memory_viewport_model_set_follow_execution(UmiDebugWorkbenchMemoryViewportModel *model, bool follow);
int umi_debug_workbench_memory_viewport_model_contains(const UmiDebugWorkbenchMemoryViewportModel *model, uint64_t address);
int umi_debug_workbench_memory_viewport_model_valid(const UmiDebugWorkbenchMemoryViewportModel *model);

#ifdef __cplusplus
}
#endif
#endif
