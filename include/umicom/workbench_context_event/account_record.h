/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/account_record.h
 *
 * PURPOSE:
 *   Define the reusable account event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_ACCOUNT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_ACCOUNT_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventAccountRecord {
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
} UmiWorkbenchContextEventAccountRecord;

void umi_workbench_context_event_account_record_init(
    UmiWorkbenchContextEventAccountRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_account_record_validate(
    const UmiWorkbenchContextEventAccountRecord *record);
UmiStatus umi_workbench_context_event_account_record_set_source(
    UmiWorkbenchContextEventAccountRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_account_record_set_subject(
    UmiWorkbenchContextEventAccountRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_account_record_set_group(
    UmiWorkbenchContextEventAccountRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_account_record_set_label(
    UmiWorkbenchContextEventAccountRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_account_record_hash(
    const UmiWorkbenchContextEventAccountRecord *record);
void umi_workbench_context_event_account_record_touch(
    UmiWorkbenchContextEventAccountRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
