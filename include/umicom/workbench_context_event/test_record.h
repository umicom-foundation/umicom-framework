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

/**
 * Represent the workbench context event test record data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context event test record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_test_record_init(
    UmiWorkbenchContextEventTestRecord *record,
    const char *record_id);
/**
 * Check that workbench context event test record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_test_record_validate(
    const UmiWorkbenchContextEventTestRecord *record);
/**
 * Provide the workbench context event test record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_test_record_set_source(
    UmiWorkbenchContextEventTestRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event test record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_test_record_set_subject(
    UmiWorkbenchContextEventTestRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event test record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_test_record_set_group(
    UmiWorkbenchContextEventTestRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event test record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_test_record_set_label(
    UmiWorkbenchContextEventTestRecord *record,
    const char *label);
/**
 * Provide the workbench context event test record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_test_record_hash(
    const UmiWorkbenchContextEventTestRecord *record);
/**
 * Provide the workbench context event test record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_event_test_record_touch(
    UmiWorkbenchContextEventTestRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
