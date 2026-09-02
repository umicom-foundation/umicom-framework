/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/source_descriptor.h
 *
 * PURPOSE:
 *   Define the reusable event source descriptor contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SOURCE_DESCRIPTOR_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SOURCE_DESCRIPTOR_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event source descriptor data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventSourceDescriptor {
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
} UmiWorkbenchContextEventSourceDescriptor;

/**
 * Initialise workbench context event source descriptor from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_event_source_descriptor_init(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *record_id);
/**
 * Check that workbench context event source descriptor satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_source_descriptor_validate(
    const UmiWorkbenchContextEventSourceDescriptor *record);
/**
 * Provide the workbench context event source descriptor set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_source_descriptor_set_source(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *source_id);
/**
 * Provide the workbench context event source descriptor set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_source_descriptor_set_subject(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *subject_id);
/**
 * Provide the workbench context event source descriptor set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_source_descriptor_set_group(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *group_id);
/**
 * Provide the workbench context event source descriptor set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_source_descriptor_set_label(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *label);
/**
 * Provide the workbench context event source descriptor hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_source_descriptor_hash(
    const UmiWorkbenchContextEventSourceDescriptor *record);
/**
 * Provide the workbench context event source descriptor touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_event_source_descriptor_touch(
    UmiWorkbenchContextEventSourceDescriptor *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
