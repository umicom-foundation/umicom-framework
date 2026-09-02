/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/variable_item.h
 *
 * PURPOSE:
 *   Represent one variable/watch value with lazy-child and change presentation state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_VARIABLE_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_VARIABLE_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench variable item data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchVariableItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchVariableItem;

/**
 * Initialise debug workbench variable item from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_workbench_variable_item_init(UmiDebugWorkbenchVariableItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench variable item set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_variable_item_set_state(UmiDebugWorkbenchVariableItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench variable item set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_workbench_variable_item_set_selected(UmiDebugWorkbenchVariableItem *model, bool selected);
/**
 * Provide the debug workbench variable item set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_variable_item_set_enabled(UmiDebugWorkbenchVariableItem *model, bool enabled);
/**
 * Check that debug workbench variable item satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_variable_item_valid(const UmiDebugWorkbenchVariableItem *model);

#ifdef __cplusplus
}
#endif
#endif
