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
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug session item data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchDebugSessionItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionItem;

/**
 * Initialise debug workbench debug session item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_session_item_init(UmiDebugWorkbenchDebugSessionItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench debug session item set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_item_set_state(UmiDebugWorkbenchDebugSessionItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench debug session item set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_debug_session_item_set_selected(UmiDebugWorkbenchDebugSessionItem *model, bool selected);
/**
 * Provide the debug workbench debug session item set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_item_set_enabled(UmiDebugWorkbenchDebugSessionItem *model, bool enabled);
/**
 * Check that debug workbench debug session item satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_session_item_valid(const UmiDebugWorkbenchDebugSessionItem *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_session_item_archive_encode(const UmiDebugWorkbenchDebugSessionItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_session_item_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugSessionItem *value);

#ifdef __cplusplus
}
#endif
#endif
