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

void umi_workbench_context_event_source_descriptor_init(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_source_descriptor_validate(
    const UmiWorkbenchContextEventSourceDescriptor *record);
UmiStatus umi_workbench_context_event_source_descriptor_set_source(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_source_descriptor_set_subject(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_source_descriptor_set_group(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_source_descriptor_set_label(
    UmiWorkbenchContextEventSourceDescriptor *record,
    const char *label);
uint64_t umi_workbench_context_event_source_descriptor_hash(
    const UmiWorkbenchContextEventSourceDescriptor *record);
void umi_workbench_context_event_source_descriptor_touch(
    UmiWorkbenchContextEventSourceDescriptor *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
