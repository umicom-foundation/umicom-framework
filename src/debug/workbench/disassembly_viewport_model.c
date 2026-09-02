/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/disassembly_viewport_model.c
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
#include "umicom/debug/workbench/disassembly_viewport_model.h"
#include <string.h>

/*
 * Initialise debug workbench disassembly viewport model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_init(UmiDebugWorkbenchDisassemblyViewportModel *model, const char *id, uint64_t start_address, uint64_t extent)
{
    UmiDebugWorkbenchRange range = {{0U, 0U}, {0U, 0U}};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || extent == 0U || UINT64_MAX - start_address < extent - 1U) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_entry_init(&model->value, id, "Debug range", "", NULL, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->start_address = start_address;
    model->extent = extent;
    model->cursor_address = start_address;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench disassembly viewport model contains operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_disassembly_viewport_model_contains(const UmiDebugWorkbenchDisassemblyViewportModel *model, uint64_t address)
{ return model != NULL && address >= model->start_address && address - model->start_address < model->extent; }
/*
 * Provide the debug workbench disassembly viewport model set cursor operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_set_cursor(UmiDebugWorkbenchDisassemblyViewportModel *model, uint64_t address)
{ /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_debug_workbench_disassembly_viewport_model_contains(model, address)) return UMI_STATUS_INVALID_ARGUMENT; model->cursor_address = address; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench disassembly viewport model set follow execution operation
 * used by this module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_set_follow_execution(UmiDebugWorkbenchDisassemblyViewportModel *model, bool follow)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; model->follow_execution = follow; model->revision++; return UMI_STATUS_OK; }
/*
 * Check that debug workbench disassembly viewport model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_disassembly_viewport_model_valid(const UmiDebugWorkbenchDisassemblyViewportModel *model)
{ return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->extent > 0U && umi_debug_workbench_disassembly_viewport_model_contains(model, model->cursor_address) && model->revision > 0U; }
