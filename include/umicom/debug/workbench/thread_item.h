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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_THREAD_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_THREAD_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench thread item data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchThreadItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchThreadItem;

/**
 * Initialise debug workbench thread item from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_workbench_thread_item_init(UmiDebugWorkbenchThreadItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench thread item set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workbench_thread_item_set_state(UmiDebugWorkbenchThreadItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench thread item set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_workbench_thread_item_set_selected(UmiDebugWorkbenchThreadItem *model, bool selected);
/**
 * Provide the debug workbench thread item set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_thread_item_set_enabled(UmiDebugWorkbenchThreadItem *model, bool enabled);
/**
 * Check that debug workbench thread item satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_thread_item_valid(const UmiDebugWorkbenchThreadItem *model);

#ifdef __cplusplus
}
#endif
#endif
