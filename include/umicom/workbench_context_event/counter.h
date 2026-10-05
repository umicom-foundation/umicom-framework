/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/counter.h
 *
 * PURPOSE:
 *   Define the reusable event counter contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_COUNTER_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_COUNTER_H

#include "umicom/workbench_context_event/event.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event counter data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventCounter {
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
} UmiWorkbenchContextEventCounter;

/**
 * Initialise workbench context event counter from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_counter_init(
    UmiWorkbenchContextEventCounter *record,
    const char *record_id);
/**
 * Check that workbench context event counter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_event_counter_validate(
    const UmiWorkbenchContextEventCounter *record);
/**
 * Provide the workbench context event counter set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_counter_set_source(
    UmiWorkbenchContextEventCounter *record,
    const char *source_id);
/**
 * Provide the workbench context event counter set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_counter_set_subject(
    UmiWorkbenchContextEventCounter *record,
    const char *subject_id);
/**
 * Provide the workbench context event counter set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_counter_set_group(
    UmiWorkbenchContextEventCounter *record,
    const char *group_id);
/**
 * Provide the workbench context event counter set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_counter_set_label(
    UmiWorkbenchContextEventCounter *record,
    const char *label);
/**
 * Provide the workbench context event counter hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_counter_hash(
    const UmiWorkbenchContextEventCounter *record);
/**
 * Provide the workbench context event counter touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_counter_touch(
    UmiWorkbenchContextEventCounter *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_event_counter_archive_encode(const UmiWorkbenchContextEventCounter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_event_counter_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextEventCounter *value);

#ifdef __cplusplus
}
#endif
#endif
