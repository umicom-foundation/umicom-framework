/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_timeline_event.h
 *
 * PURPOSE:
 *   Represent one timestamped debugger lifecycle, stop or navigation event.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_EVENT_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_EVENT_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug timeline event data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugTimelineEvent {
    UmiDebugWorkbenchEntry value;
    uint64_t sequence;
    uint64_t timestamp_ns;
    UmiDebugWorkbenchSessionPhase phase;
    uint64_t revision;
} UmiDebugWorkbenchDebugTimelineEvent;

/**
 * Initialise debug workbench debug timeline event from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_timeline_event_init(UmiDebugWorkbenchDebugTimelineEvent *model, const char *id, uint64_t sequence, uint64_t timestamp_ns, UmiDebugWorkbenchSessionPhase phase, const char *label);
/**
 * Provide the debug workbench debug timeline event retime operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_timeline_event_retime(UmiDebugWorkbenchDebugTimelineEvent *model, uint64_t timestamp_ns);
/**
 * Check that debug workbench debug timeline event satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_timeline_event_valid(const UmiDebugWorkbenchDebugTimelineEvent *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_timeline_event_archive_encode(const UmiDebugWorkbenchDebugTimelineEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_timeline_event_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugTimelineEvent *value);

#ifdef __cplusplus
}
#endif
#endif
