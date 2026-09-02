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

#ifdef __cplusplus
}
#endif
#endif
