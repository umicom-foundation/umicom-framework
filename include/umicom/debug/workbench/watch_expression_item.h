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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_WATCH_EXPRESSION_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_WATCH_EXPRESSION_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchWatchExpressionItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchWatchExpressionItem;

UmiStatus umi_debug_workbench_watch_expression_item_init(UmiDebugWorkbenchWatchExpressionItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_watch_expression_item_set_state(UmiDebugWorkbenchWatchExpressionItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_watch_expression_item_set_selected(UmiDebugWorkbenchWatchExpressionItem *model, bool selected);
UmiStatus umi_debug_workbench_watch_expression_item_set_enabled(UmiDebugWorkbenchWatchExpressionItem *model, bool enabled);
int umi_debug_workbench_watch_expression_item_valid(const UmiDebugWorkbenchWatchExpressionItem *model);

#ifdef __cplusplus
}
#endif
#endif
