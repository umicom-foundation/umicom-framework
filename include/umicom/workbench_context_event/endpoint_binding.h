/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/endpoint_binding.h
 *
 * PURPOSE:
 *   Define the reusable endpoint event binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_ENDPOINT_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_ENDPOINT_BINDING_H

#include "umicom/workbench_context_event/event.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event endpoint binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventEndpointBinding {
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
} UmiWorkbenchContextEventEndpointBinding;

/**
 * Initialise workbench context event endpoint binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_endpoint_binding_init(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *record_id);
/**
 * Check that workbench context event endpoint binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_endpoint_binding_validate(
    const UmiWorkbenchContextEventEndpointBinding *record);
/**
 * Provide the workbench context event endpoint binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_endpoint_binding_set_source(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *source_id);
/**
 * Provide the workbench context event endpoint binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_endpoint_binding_set_subject(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *subject_id);
/**
 * Provide the workbench context event endpoint binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_endpoint_binding_set_group(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *group_id);
/**
 * Provide the workbench context event endpoint binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_endpoint_binding_set_label(
    UmiWorkbenchContextEventEndpointBinding *record,
    const char *label);
/**
 * Provide the workbench context event endpoint binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_endpoint_binding_hash(
    const UmiWorkbenchContextEventEndpointBinding *record);
/**
 * Provide the workbench context event endpoint binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_endpoint_binding_touch(
    UmiWorkbenchContextEventEndpointBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_event_endpoint_binding_archive_encode(const UmiWorkbenchContextEventEndpointBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_event_endpoint_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextEventEndpointBinding *value);

#ifdef __cplusplus
}
#endif
#endif
