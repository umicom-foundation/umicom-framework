/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/watch_expression_item.h
 *
 * PURPOSE:
 *   Represent one persisted watch expression and latest evaluation status.
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
#ifndef UMICOM_DEBUG_WORKBENCH_WATCH_EXPRESSION_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_WATCH_EXPRESSION_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench watch expression item data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchWatchExpressionItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchWatchExpressionItem;

/**
 * Initialise debug workbench watch expression item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_watch_expression_item_init(UmiDebugWorkbenchWatchExpressionItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench watch expression item set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_watch_expression_item_set_state(UmiDebugWorkbenchWatchExpressionItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench watch expression item set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_watch_expression_item_set_selected(UmiDebugWorkbenchWatchExpressionItem *model, bool selected);
/**
 * Provide the debug workbench watch expression item set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_watch_expression_item_set_enabled(UmiDebugWorkbenchWatchExpressionItem *model, bool enabled);
/**
 * Check that debug workbench watch expression item satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_watch_expression_item_valid(const UmiDebugWorkbenchWatchExpressionItem *model);

#ifdef __cplusplus
}
#endif
#endif
