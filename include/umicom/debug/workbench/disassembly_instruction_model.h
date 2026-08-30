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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDisassemblyInstructionModel {
    UmiDebugWorkbenchEntry value;
    uint64_t start_address;
    uint64_t extent;
    uint64_t cursor_address;
    bool follow_execution;
    uint64_t revision;
} UmiDebugWorkbenchDisassemblyInstructionModel;

UmiStatus umi_debug_workbench_disassembly_instruction_model_init(UmiDebugWorkbenchDisassemblyInstructionModel *model, const char *id, uint64_t start_address, uint64_t extent);
UmiStatus umi_debug_workbench_disassembly_instruction_model_set_cursor(UmiDebugWorkbenchDisassemblyInstructionModel *model, uint64_t address);
UmiStatus umi_debug_workbench_disassembly_instruction_model_set_follow_execution(UmiDebugWorkbenchDisassemblyInstructionModel *model, bool follow);
int umi_debug_workbench_disassembly_instruction_model_contains(const UmiDebugWorkbenchDisassemblyInstructionModel *model, uint64_t address);
int umi_debug_workbench_disassembly_instruction_model_valid(const UmiDebugWorkbenchDisassemblyInstructionModel *model);

#ifdef __cplusplus
}
#endif
#endif
