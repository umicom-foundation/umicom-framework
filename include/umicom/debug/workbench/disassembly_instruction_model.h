/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/disassembly_instruction_model.h
 *
 * PURPOSE:
 *   Represent one decoded instruction, address and source association.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DISASSEMBLY_INSTRUCTION_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DISASSEMBLY_INSTRUCTION_MODEL_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench disassembly instruction model data shared with callers of
 * this public contract.
 */
typedef struct UmiDebugWorkbenchDisassemblyInstructionModel {
    UmiDebugWorkbenchEntry value;
    uint64_t start_address;
    uint64_t extent;
    uint64_t cursor_address;
    bool follow_execution;
    uint64_t revision;
} UmiDebugWorkbenchDisassemblyInstructionModel;

/**
 * Initialise debug workbench disassembly instruction model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_disassembly_instruction_model_init(UmiDebugWorkbenchDisassemblyInstructionModel *model, const char *id, uint64_t start_address, uint64_t extent);
/**
 * Provide the debug workbench disassembly instruction model set cursor operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_instruction_model_set_cursor(UmiDebugWorkbenchDisassemblyInstructionModel *model, uint64_t address);
/**
 * Provide the debug workbench disassembly instruction model set follow execution operation
 * used by this module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_instruction_model_set_follow_execution(UmiDebugWorkbenchDisassemblyInstructionModel *model, bool follow);
/**
 * Provide the debug workbench disassembly instruction model contains operation used by
 * this module and its client applications.
 */
int umi_debug_workbench_disassembly_instruction_model_contains(const UmiDebugWorkbenchDisassemblyInstructionModel *model, uint64_t address);
/**
 * Check that debug workbench disassembly instruction model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_disassembly_instruction_model_valid(const UmiDebugWorkbenchDisassemblyInstructionModel *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_disassembly_instruction_model_archive_encode(const UmiDebugWorkbenchDisassemblyInstructionModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_disassembly_instruction_model_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDisassemblyInstructionModel *value);

#ifdef __cplusplus
}
#endif
#endif
