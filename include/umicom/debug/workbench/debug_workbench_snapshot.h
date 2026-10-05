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

#ifdef __cplusplus
}
#endif
#endif
