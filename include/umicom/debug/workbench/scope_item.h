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

typedef struct UmiDebugWorkbenchScopeItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchScopeItem;

UmiStatus umi_debug_workbench_scope_item_init(UmiDebugWorkbenchScopeItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_scope_item_set_state(UmiDebugWorkbenchScopeItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_scope_item_set_selected(UmiDebugWorkbenchScopeItem *model, bool selected);
UmiStatus umi_debug_workbench_scope_item_set_enabled(UmiDebugWorkbenchScopeItem *model, bool enabled);
int umi_debug_workbench_scope_item_valid(const UmiDebugWorkbenchScopeItem *model);

#ifdef __cplusplus
}
#endif
#endif
