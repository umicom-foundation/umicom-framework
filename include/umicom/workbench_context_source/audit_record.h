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

/**
 * Represent the workbench context source audit record data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context source audit record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_audit_record_init(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *record_id);
/**
 * Check that workbench context source audit record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_audit_record_validate(
    const UmiWorkbenchContextSourceAuditRecord *record);
/**
 * Provide the workbench context source audit record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_audit_record_set_source(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source audit record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_audit_record_set_panel(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source audit record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_audit_record_set_subject(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source audit record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_audit_record_set_group(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source audit record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_audit_record_set_label(
    UmiWorkbenchContextSourceAuditRecord *record,
    const char *label);
/**
 * Provide the workbench context source audit record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_audit_record_hash(
    const UmiWorkbenchContextSourceAuditRecord *record);
/**
 * Provide the workbench context source audit record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_audit_record_touch(
    UmiWorkbenchContextSourceAuditRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
