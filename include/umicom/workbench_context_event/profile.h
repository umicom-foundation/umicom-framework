/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/profile.h
 *
 * PURPOSE:
 *   Define the reusable event routing profile contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PROFILE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventProfile {
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
} UmiWorkbenchContextEventProfile;

void umi_workbench_context_event_profile_init(
    UmiWorkbenchContextEventProfile *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_profile_validate(
    const UmiWorkbenchContextEventProfile *record);
UmiStatus umi_workbench_context_event_profile_set_source(
    UmiWorkbenchContextEventProfile *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_profile_set_subject(
    UmiWorkbenchContextEventProfile *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_profile_set_group(
    UmiWorkbenchContextEventProfile *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_profile_set_label(
    UmiWorkbenchContextEventProfile *record,
    const char *label);
uint64_t umi_workbench_context_event_profile_hash(
    const UmiWorkbenchContextEventProfile *record);
void umi_workbench_context_event_profile_touch(
    UmiWorkbenchContextEventProfile *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
