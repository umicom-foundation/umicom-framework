/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/call_stack_model.h
 *
 * PURPOSE:
 *   Maintain call-stack entries and the focused frame for the active thread.
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
#ifndef UMICOM_DEBUG_WORKBENCH_CALL_STACK_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_CALL_STACK_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchCallStackModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchCallStackModel;

void umi_debug_workbench_call_stack_model_init(UmiDebugWorkbenchCallStackModel *model);
UmiStatus umi_debug_workbench_call_stack_model_add(UmiDebugWorkbenchCallStackModel *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_call_stack_model_remove(UmiDebugWorkbenchCallStackModel *model, const char *id);
UmiStatus umi_debug_workbench_call_stack_model_select(UmiDebugWorkbenchCallStackModel *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_call_stack_model_find(const UmiDebugWorkbenchCallStackModel *model, const char *id);
int umi_debug_workbench_call_stack_model_valid(const UmiDebugWorkbenchCallStackModel *model);

#ifdef __cplusplus
}
#endif
#endif
