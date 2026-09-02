/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/result_record.h
 *
 * PURPOSE:
 *   Define the reusable provider result record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_RESULT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_RESULT_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider result record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderResultRecord {
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
} UmiWorkbenchSelectionProviderResultRecord;

/**
 * Initialise workbench selection provider result record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_result_record_init(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider result record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_result_record_validate(
    const UmiWorkbenchSelectionProviderResultRecord *record);
/**
 * Provide the workbench selection provider result record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_result_record_set_provider(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider result record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_result_record_set_source(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider result record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_result_record_set_subject(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider result record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_result_record_set_related(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider result record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_result_record_set_group(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider result record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_result_record_set_description(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider result record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_result_record_hash(
    const UmiWorkbenchSelectionProviderResultRecord *record);
/**
 * Provide the workbench selection provider result record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_result_record_touch(
    UmiWorkbenchSelectionProviderResultRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
