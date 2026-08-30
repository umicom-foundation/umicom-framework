/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/audit_record.h
 *
 * PURPOSE:
 *   Define the reusable source audit record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_AUDIT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_AUDIT_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceAuditRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceAuditRecord;

void umi_workbench_context_source_audit_record_init(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_audit_record_validate(
    const UmiWorkbenchContextSourceAuditRecord *record);
UmiStatus umi_workbench_context_source_audit_record_set_source(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_audit_record_set_panel(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_audit_record_set_subject(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_audit_record_set_group(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_audit_record_set_label(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_audit_record_hash(
    const UmiWorkbenchContextSourceAuditRecord *record);
void umi_workbench_context_source_audit_record_touch(
    UmiWorkbenchContextSourceAuditRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
