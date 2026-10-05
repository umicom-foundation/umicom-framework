/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/breakpoint_item.h
 *
 * PURPOSE:
 *   Represent one source, function, data, exception, instruction or log breakpoint.
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
#ifndef UMICOM_DEBUG_WORKBENCH_BREAKPOINT_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_BREAKPOINT_ITEM_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench breakpoint item data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchBreakpointItem {
    UmiDebugWorkbenchEntry value;
    UmiDebugWorkbenchBreakpointKind kind;
    bool enabled;
    bool verified;
    uint64_t hit_count;
    uint64_t revision;
} UmiDebugWorkbenchBreakpointItem;

/**
 * Initialise debug workbench breakpoint item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_breakpoint_item_init(UmiDebugWorkbenchBreakpointItem *model, const char *id, UmiDebugWorkbenchBreakpointKind kind, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench breakpoint item set verification operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_item_set_verification(UmiDebugWorkbenchBreakpointItem *model, bool verified);
/**
 * Provide the debug workbench breakpoint item record hit operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_item_record_hit(UmiDebugWorkbenchBreakpointItem *model);
/**
 * Provide the debug workbench breakpoint item set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_item_set_enabled(UmiDebugWorkbenchBreakpointItem *model, bool enabled);
/**
 * Check that debug workbench breakpoint item satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_breakpoint_item_valid(const UmiDebugWorkbenchBreakpointItem *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_breakpoint_item_archive_encode(const UmiDebugWorkbenchBreakpointItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_breakpoint_item_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchBreakpointItem *value);

#ifdef __cplusplus
}
#endif
#endif
