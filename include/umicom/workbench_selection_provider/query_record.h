/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/query_record.h
 *
 * PURPOSE:
 *   Define the reusable provider query record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_QUERY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_QUERY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider query record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderQueryRecord {
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
} UmiWorkbenchSelectionProviderQueryRecord;

/**
 * Initialise workbench selection provider query record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_query_record_init(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider query record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_query_record_validate(
    const UmiWorkbenchSelectionProviderQueryRecord *record);
/**
 * Provide the workbench selection provider query record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_query_record_set_provider(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider query record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_query_record_set_source(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider query record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_query_record_set_subject(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider query record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_query_record_set_related(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider query record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_query_record_set_group(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider query record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_query_record_set_description(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider query record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_query_record_hash(
    const UmiWorkbenchSelectionProviderQueryRecord *record);
/**
 * Provide the workbench selection provider query record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_query_record_touch(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
