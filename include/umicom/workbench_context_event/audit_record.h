/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/audit_record.h
 *
 * PURPOSE:
 *   Define the reusable event audit record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_AUDIT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_AUDIT_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventAuditRecord {
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
} UmiWorkbenchContextEventAuditRecord;

void umi_workbench_context_event_audit_record_init(
    UmiWorkbenchContextEventAuditRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_audit_record_validate(
    const UmiWorkbenchContextEventAuditRecord *record);
UmiStatus umi_workbench_context_event_audit_record_set_source(
    UmiWorkbenchContextEventAuditRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_audit_record_set_subject(
    UmiWorkbenchContextEventAuditRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_audit_record_set_group(
    UmiWorkbenchContextEventAuditRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_audit_record_set_label(
    UmiWorkbenchContextEventAuditRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_audit_record_hash(
    const UmiWorkbenchContextEventAuditRecord *record);
void umi_workbench_context_event_audit_record_touch(
    UmiWorkbenchContextEventAuditRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
