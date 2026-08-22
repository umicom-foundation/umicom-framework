/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/policy.h
 *
 * PURPOSE:
 *   Define the reusable event policy contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_POLICY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventPolicy {
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
} UmiWorkbenchContextEventPolicy;

void umi_workbench_context_event_policy_init(
    UmiWorkbenchContextEventPolicy *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_policy_validate(
    const UmiWorkbenchContextEventPolicy *record);
UmiStatus umi_workbench_context_event_policy_set_source(
    UmiWorkbenchContextEventPolicy *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_policy_set_subject(
    UmiWorkbenchContextEventPolicy *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_policy_set_group(
    UmiWorkbenchContextEventPolicy *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_policy_set_label(
    UmiWorkbenchContextEventPolicy *record,
    const char *label);
uint64_t umi_workbench_context_event_policy_hash(
    const UmiWorkbenchContextEventPolicy *record);
void umi_workbench_context_event_policy_touch(
    UmiWorkbenchContextEventPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
