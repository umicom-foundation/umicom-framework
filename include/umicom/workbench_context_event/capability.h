/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/capability.h
 *
 * PURPOSE:
 *   Define the reusable event capability contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_CAPABILITY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_CAPABILITY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventCapability {
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
} UmiWorkbenchContextEventCapability;

void umi_workbench_context_event_capability_init(
    UmiWorkbenchContextEventCapability *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_capability_validate(
    const UmiWorkbenchContextEventCapability *record);
UmiStatus umi_workbench_context_event_capability_set_source(
    UmiWorkbenchContextEventCapability *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_capability_set_subject(
    UmiWorkbenchContextEventCapability *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_capability_set_group(
    UmiWorkbenchContextEventCapability *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_capability_set_label(
    UmiWorkbenchContextEventCapability *record,
    const char *label);
uint64_t umi_workbench_context_event_capability_hash(
    const UmiWorkbenchContextEventCapability *record);
void umi_workbench_context_event_capability_touch(
    UmiWorkbenchContextEventCapability *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
