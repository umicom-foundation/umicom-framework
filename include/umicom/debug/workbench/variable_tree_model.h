/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/variable_tree_model.h
 *
 * PURPOSE:
 *   Maintain flattened visible variable nodes for a toolkit-neutral tree projection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_VARIABLE_TREE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_VARIABLE_TREE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchVariableTreeModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchVariableTreeModel;

void umi_debug_workbench_variable_tree_model_init(UmiDebugWorkbenchVariableTreeModel *model);
UmiStatus umi_debug_workbench_variable_tree_model_add(UmiDebugWorkbenchVariableTreeModel *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_variable_tree_model_remove(UmiDebugWorkbenchVariableTreeModel *model, const char *id);
UmiStatus umi_debug_workbench_variable_tree_model_select(UmiDebugWorkbenchVariableTreeModel *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_variable_tree_model_find(const UmiDebugWorkbenchVariableTreeModel *model, const char *id);
int umi_debug_workbench_variable_tree_model_valid(const UmiDebugWorkbenchVariableTreeModel *model);

#ifdef __cplusplus
}
#endif
#endif
