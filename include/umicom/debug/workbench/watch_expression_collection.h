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

typedef struct UmiDebugWorkbenchWatchExpressionCollection {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchWatchExpressionCollection;

void umi_debug_workbench_watch_expression_collection_init(UmiDebugWorkbenchWatchExpressionCollection *model);
UmiStatus umi_debug_workbench_watch_expression_collection_add(UmiDebugWorkbenchWatchExpressionCollection *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_watch_expression_collection_remove(UmiDebugWorkbenchWatchExpressionCollection *model, const char *id);
UmiStatus umi_debug_workbench_watch_expression_collection_select(UmiDebugWorkbenchWatchExpressionCollection *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_watch_expression_collection_find(const UmiDebugWorkbenchWatchExpressionCollection *model, const char *id);
int umi_debug_workbench_watch_expression_collection_valid(const UmiDebugWorkbenchWatchExpressionCollection *model);

#ifdef __cplusplus
}
#endif
#endif
