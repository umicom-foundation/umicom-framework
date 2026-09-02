/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/disassembly_viewport_model.h
 *
 * PURPOSE:
 *   Track disassembly viewport range, active instruction and follow-PC state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DISASSEMBLY_VIEWPORT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DISASSEMBLY_VIEWPORT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench disassembly viewport model data shared with callers of
 * this public contract.
 */
typedef struct UmiDebugWorkbenchDisassemblyViewportModel {
    UmiDebugWorkbenchEntry value;
    uint64_t start_address;
    uint64_t extent;
    uint64_t cursor_address;
    bool follow_execution;
    uint64_t revision;
} UmiDebugWorkbenchDisassemblyViewportModel;

/**
 * Initialise debug workbench disassembly viewport model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_init(UmiDebugWorkbenchDisassemblyViewportModel *model, const char *id, uint64_t start_address, uint64_t extent);
/**
 * Provide the debug workbench disassembly viewport model set cursor operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_set_cursor(UmiDebugWorkbenchDisassemblyViewportModel *model, uint64_t address);
/**
 * Provide the debug workbench disassembly viewport model set follow execution operation
 * used by this module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_set_follow_execution(UmiDebugWorkbenchDisassemblyViewportModel *model, bool follow);
/**
 * Provide the debug workbench disassembly viewport model contains operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_disassembly_viewport_model_contains(const UmiDebugWorkbenchDisassemblyViewportModel *model, uint64_t address);
/**
 * Check that debug workbench disassembly viewport model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_disassembly_viewport_model_valid(const UmiDebugWorkbenchDisassemblyViewportModel *model);

#ifdef __cplusplus
}
#endif
#endif
