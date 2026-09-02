/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/source_row_record.h
 *
 * PURPOSE:
 *   Define the reusable structured source-control row record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_ROW_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_ROW_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider source row record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSourceRowRecord {
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
} UmiWorkbenchSelectionProviderSourceRowRecord;

/**
 * Initialise workbench selection provider source row record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_source_row_record_init(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider source row record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_source_row_record_validate(
    const UmiWorkbenchSelectionProviderSourceRowRecord *record);
/**
 * Provide the workbench selection provider source row record set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_row_record_set_provider(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider source row record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_row_record_set_source(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider source row record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_row_record_set_subject(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider source row record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_row_record_set_related(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider source row record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_row_record_set_group(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider source row record set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_row_record_set_description(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider source row record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_source_row_record_hash(
    const UmiWorkbenchSelectionProviderSourceRowRecord *record);
/**
 * Provide the workbench selection provider source row record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_source_row_record_touch(
    UmiWorkbenchSelectionProviderSourceRowRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
