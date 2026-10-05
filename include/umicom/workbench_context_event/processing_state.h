/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/processing_state.h
 *
 * PURPOSE:
 *   Define the reusable event processing state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PROCESSING_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PROCESSING_STATE_H

#include "umicom/workbench_context_event/event.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event processing state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventProcessingState {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventProcessingState;

/**
 * Initialise workbench context event processing state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_processing_state_init(
    UmiWorkbenchContextEventProcessingState *record,
    const char *record_id);
/**
 * Check that workbench context event processing state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_processing_state_validate(
    const UmiWorkbenchContextEventProcessingState *record);
/**
 * Provide the workbench context event processing state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_processing_state_set_source(
    UmiWorkbenchContextEventProcessingState *record,
    const char *source_id);
/**
 * Provide the workbench context event processing state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_processing_state_set_subject(
    UmiWorkbenchContextEventProcessingState *record,
    const char *subject_id);
/**
 * Provide the workbench context event processing state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_processing_state_set_group(
    UmiWorkbenchContextEventProcessingState *record,
    const char *group_id);
/**
 * Provide the workbench context event processing state set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_processing_state_set_label(
    UmiWorkbenchContextEventProcessingState *record,
    const char *label);
/**
 * Provide the workbench context event processing state hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_processing_state_hash(
    const UmiWorkbenchContextEventProcessingState *record);
/**
 * Provide the workbench context event processing state touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_processing_state_touch(
    UmiWorkbenchContextEventProcessingState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_event_processing_state_archive_encode(const UmiWorkbenchContextEventProcessingState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_event_processing_state_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextEventProcessingState *value);

#ifdef __cplusplus
}
#endif
#endif
