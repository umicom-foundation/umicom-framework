/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/scope_item.h
 *
 * PURPOSE:
 *   Represent one variables scope such as locals, arguments, globals or registers.
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
#ifndef UMICOM_DEBUG_WORKBENCH_SCOPE_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_SCOPE_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench scope item data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchScopeItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchScopeItem;

/**
 * Initialise debug workbench scope item from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_workbench_scope_item_init(UmiDebugWorkbenchScopeItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench scope item set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workbench_scope_item_set_state(UmiDebugWorkbenchScopeItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench scope item set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_workbench_scope_item_set_selected(UmiDebugWorkbenchScopeItem *model, bool selected);
/**
 * Provide the debug workbench scope item set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workbench_scope_item_set_enabled(UmiDebugWorkbenchScopeItem *model, bool enabled);
/**
 * Check that debug workbench scope item satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_scope_item_valid(const UmiDebugWorkbenchScopeItem *model);

#ifdef __cplusplus
}
#endif
#endif
