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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_SCOPE_TREE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_SCOPE_TREE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchScopeTreeModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchScopeTreeModel;

void umi_debug_workbench_scope_tree_model_init(UmiDebugWorkbenchScopeTreeModel *model);
UmiStatus umi_debug_workbench_scope_tree_model_add(UmiDebugWorkbenchScopeTreeModel *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_scope_tree_model_remove(UmiDebugWorkbenchScopeTreeModel *model, const char *id);
UmiStatus umi_debug_workbench_scope_tree_model_select(UmiDebugWorkbenchScopeTreeModel *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_scope_tree_model_find(const UmiDebugWorkbenchScopeTreeModel *model, const char *id);
int umi_debug_workbench_scope_tree_model_valid(const UmiDebugWorkbenchScopeTreeModel *model);

#ifdef __cplusplus
}
#endif
#endif
