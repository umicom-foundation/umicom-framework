/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/profile_binding.h
 *
 * PURPOSE:
 *   Define the reusable profile event binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PROFILE_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PROFILE_BINDING_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventProfileBinding {
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
} UmiWorkbenchContextEventProfileBinding;

void umi_workbench_context_event_profile_binding_init(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_profile_binding_validate(
    const UmiWorkbenchContextEventProfileBinding *record);
UmiStatus umi_workbench_context_event_profile_binding_set_source(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_profile_binding_set_subject(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_profile_binding_set_group(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_profile_binding_set_label(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *label);
uint64_t umi_workbench_context_event_profile_binding_hash(
    const UmiWorkbenchContextEventProfileBinding *record);
void umi_workbench_context_event_profile_binding_touch(
    UmiWorkbenchContextEventProfileBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
