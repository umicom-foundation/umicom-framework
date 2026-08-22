/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/session_record.h
 *
 * PURPOSE:
 *   Define the reusable event session record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SESSION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SESSION_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventSessionRecord {
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
} UmiWorkbenchContextEventSessionRecord;

void umi_workbench_context_event_session_record_init(
    UmiWorkbenchContextEventSessionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_session_record_validate(
    const UmiWorkbenchContextEventSessionRecord *record);
UmiStatus umi_workbench_context_event_session_record_set_source(
    UmiWorkbenchContextEventSessionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_session_record_set_subject(
    UmiWorkbenchContextEventSessionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_session_record_set_group(
    UmiWorkbenchContextEventSessionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_session_record_set_label(
    UmiWorkbenchContextEventSessionRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_session_record_hash(
    const UmiWorkbenchContextEventSessionRecord *record);
void umi_workbench_context_event_session_record_touch(
    UmiWorkbenchContextEventSessionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
