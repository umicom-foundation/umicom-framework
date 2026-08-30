/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/disassembly_instruction_model.c
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
#include "umicom/debug/workbench/disassembly_instruction_model.h"
#include <string.h>

UmiStatus umi_debug_workbench_disassembly_instruction_model_init(UmiDebugWorkbenchDisassemblyInstructionModel *model, const char *id, uint64_t start_address, uint64_t extent)
{
    UmiDebugWorkbenchRange range = {{0U, 0U}, {0U, 0U}};
    if (model == NULL || extent == 0U || UINT64_MAX - start_address < extent - 1U) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    if (umi_debug_workbench_entry_init(&model->value, id, "Debug range", "", NULL, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->start_address = start_address;
    model->extent = extent;
    model->cursor_address = start_address;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_disassembly_instruction_model_contains(const UmiDebugWorkbenchDisassemblyInstructionModel *model, uint64_t address)
{ return model != NULL && address >= model->start_address && address - model->start_address < model->extent; }
UmiStatus umi_debug_workbench_disassembly_instruction_model_set_cursor(UmiDebugWorkbenchDisassemblyInstructionModel *model, uint64_t address)
{ if (!umi_debug_workbench_disassembly_instruction_model_contains(model, address)) return UMI_STATUS_INVALID_ARGUMENT; model->cursor_address = address; model->revision++; return UMI_STATUS_OK; }
UmiStatus umi_debug_workbench_disassembly_instruction_model_set_follow_execution(UmiDebugWorkbenchDisassemblyInstructionModel *model, bool follow)
{ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; model->follow_execution = follow; model->revision++; return UMI_STATUS_OK; }
int umi_debug_workbench_disassembly_instruction_model_valid(const UmiDebugWorkbenchDisassemblyInstructionModel *model)
{ return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->extent > 0U && umi_debug_workbench_disassembly_instruction_model_contains(model, model->cursor_address) && model->revision > 0U; }
