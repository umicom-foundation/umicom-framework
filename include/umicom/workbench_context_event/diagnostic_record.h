/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/diagnostic_record.h
 *
 * PURPOSE:
 *   Define the reusable diagnostic event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event diagnostic record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventDiagnosticRecord {
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
} UmiWorkbenchContextEventDiagnosticRecord;

/**
 * Initialise workbench context event diagnostic record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_event_diagnostic_record_init(
    UmiWorkbenchContextEventDiagnosticRecord *record,
    const char *record_id);
/**
 * Check that workbench context event diagnostic record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_diagnostic_record_validate(
    const UmiWorkbenchContextEventDiagnosticRecord *record);
/**
 * Provide the workbench context event diagnostic record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_record_set_source(
    UmiWorkbenchContextEventDiagnosticRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event diagnostic record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_record_set_subject(
    UmiWorkbenchContextEventDiagnosticRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event diagnostic record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_record_set_group(
    UmiWorkbenchContextEventDiagnosticRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event diagnostic record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_record_set_label(
    UmiWorkbenchContextEventDiagnosticRecord *record,
    const char *label);
/**
 * Provide the workbench context event diagnostic record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_diagnostic_record_hash(
    const UmiWorkbenchContextEventDiagnosticRecord *record);
/**
 * Provide the workbench context event diagnostic record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_event_diagnostic_record_touch(
    UmiWorkbenchContextEventDiagnosticRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
