/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/event_binding.h
 *
 * PURPOSE:
 *   Define the reusable event-to-context binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_BINDING_H

#include "umicom/workbench_context_event/event.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event event binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventEventBinding {
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
} UmiWorkbenchContextEventEventBinding;

/**
 * Initialise workbench context event event binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_event_binding_init(
    UmiWorkbenchContextEventEventBinding *record,
    const char *record_id);
/**
 * Check that workbench context event event binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_event_binding_validate(
    const UmiWorkbenchContextEventEventBinding *record);
/**
 * Provide the workbench context event event binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_event_binding_set_source(
    UmiWorkbenchContextEventEventBinding *record,
    const char *source_id);
/**
 * Provide the workbench context event event binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_event_binding_set_subject(
    UmiWorkbenchContextEventEventBinding *record,
    const char *subject_id);
/**
 * Provide the workbench context event event binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_event_binding_set_group(
    UmiWorkbenchContextEventEventBinding *record,
    const char *group_id);
/**
 * Provide the workbench context event event binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_event_binding_set_label(
    UmiWorkbenchContextEventEventBinding *record,
    const char *label);
/**
 * Provide the workbench context event event binding hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_event_binding_hash(
    const UmiWorkbenchContextEventEventBinding *record);
/**
 * Provide the workbench context event event binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_event_binding_touch(
    UmiWorkbenchContextEventEventBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_event_event_binding_archive_encode(const UmiWorkbenchContextEventEventBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_event_event_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextEventEventBinding *value);

#ifdef __cplusplus
}
#endif
#endif
