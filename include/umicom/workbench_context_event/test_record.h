/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/test_record.h
 *
 * PURPOSE:
 *   Define the reusable test event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_TEST_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_TEST_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventTestRecord {
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
} UmiWorkbenchContextEventTestRecord;

void umi_workbench_context_event_test_record_init(
    UmiWorkbenchContextEventTestRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_test_record_validate(
    const UmiWorkbenchContextEventTestRecord *record);
UmiStatus umi_workbench_context_event_test_record_set_source(
    UmiWorkbenchContextEventTestRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_test_record_set_subject(
    UmiWorkbenchContextEventTestRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_test_record_set_group(
    UmiWorkbenchContextEventTestRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_test_record_set_label(
    UmiWorkbenchContextEventTestRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_test_record_hash(
    const UmiWorkbenchContextEventTestRecord *record);
void umi_workbench_context_event_test_record_touch(
    UmiWorkbenchContextEventTestRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
