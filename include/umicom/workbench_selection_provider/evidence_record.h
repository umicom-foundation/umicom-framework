/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/evidence_record.h
 *
 * PURPOSE:
 *   Define the reusable provider evidence record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_EVIDENCE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_EVIDENCE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider evidence record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderEvidenceRecord {
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
} UmiWorkbenchSelectionProviderEvidenceRecord;

/**
 * Initialise workbench selection provider evidence record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_evidence_record_init(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider evidence record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_evidence_record_validate(
    const UmiWorkbenchSelectionProviderEvidenceRecord *record);
/**
 * Provide the workbench selection provider evidence record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_evidence_record_set_provider(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider evidence record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_evidence_record_set_source(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider evidence record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_evidence_record_set_subject(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider evidence record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_evidence_record_set_related(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider evidence record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_evidence_record_set_group(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider evidence record set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_evidence_record_set_description(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider evidence record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_evidence_record_hash(
    const UmiWorkbenchSelectionProviderEvidenceRecord *record);
/**
 * Provide the workbench selection provider evidence record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_evidence_record_touch(
    UmiWorkbenchSelectionProviderEvidenceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
