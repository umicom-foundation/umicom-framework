/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/loaded_source_item.h
 *
 * PURPOSE:
 *   Represent one loaded source known by the adapter or runtime.
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
#ifndef UMICOM_DEBUG_WORKBENCH_LOADED_SOURCE_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_LOADED_SOURCE_ITEM_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench loaded source item data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchLoadedSourceItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchLoadedSourceItem;

/**
 * Initialise debug workbench loaded source item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_loaded_source_item_init(UmiDebugWorkbenchLoadedSourceItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench loaded source item set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_loaded_source_item_set_state(UmiDebugWorkbenchLoadedSourceItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench loaded source item set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_loaded_source_item_set_selected(UmiDebugWorkbenchLoadedSourceItem *model, bool selected);
/**
 * Provide the debug workbench loaded source item set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_loaded_source_item_set_enabled(UmiDebugWorkbenchLoadedSourceItem *model, bool enabled);
/**
 * Check that debug workbench loaded source item satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_loaded_source_item_valid(const UmiDebugWorkbenchLoadedSourceItem *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_loaded_source_item_archive_encode(const UmiDebugWorkbenchLoadedSourceItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_loaded_source_item_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchLoadedSourceItem *value);

#ifdef __cplusplus
}
#endif
#endif
