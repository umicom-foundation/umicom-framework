/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_status_model.h
 *
 * PURPOSE:
 *   Aggregate active session, stop reason and inspection-count status for workbench chrome.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_STATUS_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_STATUS_MODEL_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug status model data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchDebugStatusModel {
    char session_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    char stop_reason[UMI_DEBUG_WORKBENCH_TEXT_CAPACITY];
    UmiDebugWorkbenchSessionPhase phase;
    uint32_t thread_count;
    uint32_t frame_count;
    uint32_t variable_count;
    uint64_t revision;
} UmiDebugWorkbenchDebugStatusModel;

/**
 * Initialise debug workbench debug status model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_status_model_init(UmiDebugWorkbenchDebugStatusModel *model, const char *session_id);
/**
 * Provide the debug workbench debug status model update operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_debug_status_model_update(UmiDebugWorkbenchDebugStatusModel *model, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t threads, uint32_t frames, uint32_t variables);
/**
 * Check that debug workbench debug status model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_status_model_valid(const UmiDebugWorkbenchDebugStatusModel *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_status_model_archive_encode(const UmiDebugWorkbenchDebugStatusModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_status_model_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugStatusModel *value);

#ifdef __cplusplus
}
#endif
#endif
