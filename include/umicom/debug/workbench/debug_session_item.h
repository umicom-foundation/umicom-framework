/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_session_item.h
 *
 * PURPOSE:
 *   Represent one live or historical debug session in the workbench.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugSessionItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionItem;

UmiStatus umi_debug_workbench_debug_session_item_init(UmiDebugWorkbenchDebugSessionItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_session_item_set_state(UmiDebugWorkbenchDebugSessionItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_session_item_set_selected(UmiDebugWorkbenchDebugSessionItem *model, bool selected);
UmiStatus umi_debug_workbench_debug_session_item_set_enabled(UmiDebugWorkbenchDebugSessionItem *model, bool enabled);
int umi_debug_workbench_debug_session_item_valid(const UmiDebugWorkbenchDebugSessionItem *model);

#ifdef __cplusplus
}
#endif
#endif
