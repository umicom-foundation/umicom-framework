/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/audit_record.h
 *
 * PURPOSE:
 *   Define the reusable provider audit record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_AUDIT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_AUDIT_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider audit record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderAuditRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderAuditRecord;

/**
 * Initialise workbench selection provider audit record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_audit_record_init(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider audit record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_audit_record_validate(
    const UmiWorkbenchSelectionProviderAuditRecord *record);
/**
 * Provide the workbench selection provider audit record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_audit_record_set_provider(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider audit record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_audit_record_set_source(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider audit record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_audit_record_set_subject(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider audit record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_audit_record_set_related(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider audit record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_audit_record_set_group(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider audit record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_audit_record_set_description(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider audit record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_audit_record_hash(
    const UmiWorkbenchSelectionProviderAuditRecord *record);
/**
 * Provide the workbench selection provider audit record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_audit_record_touch(
    UmiWorkbenchSelectionProviderAuditRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
