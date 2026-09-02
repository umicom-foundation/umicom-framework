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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_VARIABLE_TREE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_VARIABLE_TREE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench variable tree model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchVariableTreeModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchVariableTreeModel;

/**
 * Initialise debug workbench variable tree model from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_variable_tree_model_init(UmiDebugWorkbenchVariableTreeModel *model);
/**
 * Add debug workbench variable tree model only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_debug_workbench_variable_tree_model_add(UmiDebugWorkbenchVariableTreeModel *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench variable tree model while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_workbench_variable_tree_model_remove(UmiDebugWorkbenchVariableTreeModel *model, const char *id);
/**
 * Provide the debug workbench variable tree model select operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_variable_tree_model_select(UmiDebugWorkbenchVariableTreeModel *model, const char *id);
/**
 * Find debug workbench variable tree model while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_variable_tree_model_find(const UmiDebugWorkbenchVariableTreeModel *model, const char *id);
/**
 * Check that debug workbench variable tree model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_variable_tree_model_valid(const UmiDebugWorkbenchVariableTreeModel *model);

#ifdef __cplusplus
}
#endif
#endif
