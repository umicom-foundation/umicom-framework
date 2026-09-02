/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/scope_tree_model.h
 *
 * PURPOSE:
 *   Maintain debugger scope-tree roots and expansion selection state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_SCOPE_TREE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_SCOPE_TREE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench scope tree model data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchScopeTreeModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchScopeTreeModel;

/**
 * Initialise debug workbench scope tree model from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_scope_tree_model_init(UmiDebugWorkbenchScopeTreeModel *model);
/**
 * Add debug workbench scope tree model only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_debug_workbench_scope_tree_model_add(UmiDebugWorkbenchScopeTreeModel *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench scope tree model while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_debug_workbench_scope_tree_model_remove(UmiDebugWorkbenchScopeTreeModel *model, const char *id);
/**
 * Provide the debug workbench scope tree model select operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_scope_tree_model_select(UmiDebugWorkbenchScopeTreeModel *model, const char *id);
/**
 * Find debug workbench scope tree model while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_scope_tree_model_find(const UmiDebugWorkbenchScopeTreeModel *model, const char *id);
/**
 * Check that debug workbench scope tree model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_scope_tree_model_valid(const UmiDebugWorkbenchScopeTreeModel *model);

#ifdef __cplusplus
}
#endif
#endif
