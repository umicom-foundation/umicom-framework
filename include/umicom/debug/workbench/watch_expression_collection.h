/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/watch_expression_collection.h
 *
 * PURPOSE:
 *   Maintain ordered watch expressions and active selection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_WATCH_EXPRESSION_COLLECTION_H
#define UMICOM_DEBUG_WORKBENCH_WATCH_EXPRESSION_COLLECTION_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench watch expression collection data shared with callers of
 * this public contract.
 */
typedef struct UmiDebugWorkbenchWatchExpressionCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchWatchExpressionCollection;

/**
 * Initialise debug workbench watch expression collection from caller-provided values so
 * later operations receive a known state.
 */
void umi_debug_workbench_watch_expression_collection_init(UmiDebugWorkbenchWatchExpressionCollection *model);
/**
 * Add debug workbench watch expression collection only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_debug_workbench_watch_expression_collection_add(UmiDebugWorkbenchWatchExpressionCollection *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench watch expression collection while keeping the remaining records
 * in a valid and discoverable state.
 */
UmiStatus umi_debug_workbench_watch_expression_collection_remove(UmiDebugWorkbenchWatchExpressionCollection *model, const char *id);
/**
 * Provide the debug workbench watch expression collection select operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_watch_expression_collection_select(UmiDebugWorkbenchWatchExpressionCollection *model, const char *id);
/**
 * Find debug workbench watch expression collection while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_watch_expression_collection_find(const UmiDebugWorkbenchWatchExpressionCollection *model, const char *id);
/**
 * Check that debug workbench watch expression collection satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_watch_expression_collection_valid(const UmiDebugWorkbenchWatchExpressionCollection *model);

#ifdef __cplusplus
}
#endif
#endif
