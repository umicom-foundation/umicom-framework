/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/application_binding.h
 *
 * PURPOSE:
 *   Define the reusable application event binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_APPLICATION_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_APPLICATION_BINDING_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event application binding data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextEventApplicationBinding {
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
} UmiWorkbenchContextEventApplicationBinding;

/**
 * Initialise workbench context event application binding from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_event_application_binding_init(
    UmiWorkbenchContextEventApplicationBinding *record,
    const char *record_id);
/**
 * Check that workbench context event application binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_application_binding_validate(
    const UmiWorkbenchContextEventApplicationBinding *record);
/**
 * Provide the workbench context event application binding set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_event_application_binding_set_source(
    UmiWorkbenchContextEventApplicationBinding *record,
    const char *source_id);
/**
 * Provide the workbench context event application binding set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_event_application_binding_set_subject(
    UmiWorkbenchContextEventApplicationBinding *record,
    const char *subject_id);
/**
 * Provide the workbench context event application binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_application_binding_set_group(
    UmiWorkbenchContextEventApplicationBinding *record,
    const char *group_id);
/**
 * Provide the workbench context event application binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_application_binding_set_label(
    UmiWorkbenchContextEventApplicationBinding *record,
    const char *label);
/**
 * Provide the workbench context event application binding hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_event_application_binding_hash(
    const UmiWorkbenchContextEventApplicationBinding *record);
/**
 * Provide the workbench context event application binding touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_event_application_binding_touch(
    UmiWorkbenchContextEventApplicationBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
