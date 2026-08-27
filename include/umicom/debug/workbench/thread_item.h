/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/thread_item.h
 *
 * PURPOSE:
 *   Represent one debuggee thread with stable selection and status state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_THREAD_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_THREAD_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchThreadItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchThreadItem;

UmiStatus umi_debug_workbench_thread_item_init(UmiDebugWorkbenchThreadItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_thread_item_set_state(UmiDebugWorkbenchThreadItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_thread_item_set_selected(UmiDebugWorkbenchThreadItem *model, bool selected);
UmiStatus umi_debug_workbench_thread_item_set_enabled(UmiDebugWorkbenchThreadItem *model, bool enabled);
int umi_debug_workbench_thread_item_valid(const UmiDebugWorkbenchThreadItem *model);

#ifdef __cplusplus
}
#endif
#endif
