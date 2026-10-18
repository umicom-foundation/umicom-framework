/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_workbench_snapshot.h
 *
 * PURPOSE:
 *   Capture the aggregate debugger workbench selection and visible-state summary.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_WORKBENCH_SNAPSHOT_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_WORKBENCH_SNAPSHOT_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug workbench snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugWorkbenchSnapshot {
    char active_session_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    char active_item_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    uint32_t session_count;
    uint32_t breakpoint_count;
    uint32_t thread_count;
    uint32_t watch_count;
    uint64_t generation;
} UmiDebugWorkbenchDebugWorkbenchSnapshot;

/**
 * Initialise debug workbench debug workbench snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_workbench_snapshot_init(UmiDebugWorkbenchDebugWorkbenchSnapshot *model);
/**
 * Provide the debug workbench debug workbench snapshot capture operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_snapshot_capture(UmiDebugWorkbenchDebugWorkbenchSnapshot *model, const char *session_id, const char *item_id, uint32_t sessions, uint32_t breakpoints, uint32_t threads, uint32_t watches);
/**
 * Check that debug workbench debug workbench snapshot satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_workbench_snapshot_valid(const UmiDebugWorkbenchDebugWorkbenchSnapshot *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_workbench_snapshot_archive_encode(const UmiDebugWorkbenchDebugWorkbenchSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_workbench_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugWorkbenchSnapshot *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDebugWorkbenchDebugWorkbenchSnapshot object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_debug_workbench_debug_workbench_snapshot_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugWorkbenchSnapshot *workspace, UmiDebugWorkbenchDebugWorkbenchSnapshot *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_debug_workbench_debug_workbench_snapshot_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDebugWorkbenchDebugWorkbenchSnapshot *expected, UmiDebugWorkbenchDebugWorkbenchSnapshot *value);


#ifdef __cplusplus
}
#endif
#endif
