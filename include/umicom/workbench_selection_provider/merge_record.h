/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/merge_record.h
 *
 * PURPOSE:
 *   Define the reusable provider merge record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_MERGE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_MERGE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider merge record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderMergeRecord {
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
} UmiWorkbenchSelectionProviderMergeRecord;

/**
 * Initialise workbench selection provider merge record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_merge_record_init(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider merge record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_merge_record_validate(
    const UmiWorkbenchSelectionProviderMergeRecord *record);
/**
 * Provide the workbench selection provider merge record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_merge_record_set_provider(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider merge record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_merge_record_set_source(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider merge record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_merge_record_set_subject(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider merge record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_merge_record_set_related(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider merge record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_merge_record_set_group(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider merge record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_merge_record_set_description(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider merge record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_merge_record_hash(
    const UmiWorkbenchSelectionProviderMergeRecord *record);
/**
 * Provide the workbench selection provider merge record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_merge_record_touch(
    UmiWorkbenchSelectionProviderMergeRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
