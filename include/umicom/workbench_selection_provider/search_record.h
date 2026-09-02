/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/search_record.h
 *
 * PURPOSE:
 *   Define the reusable provider search record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SEARCH_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SEARCH_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider search record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSearchRecord {
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
} UmiWorkbenchSelectionProviderSearchRecord;

/**
 * Initialise workbench selection provider search record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_search_record_init(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider search record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_search_record_validate(
    const UmiWorkbenchSelectionProviderSearchRecord *record);
/**
 * Provide the workbench selection provider search record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_search_record_set_provider(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider search record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_search_record_set_source(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider search record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_search_record_set_subject(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider search record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_search_record_set_related(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider search record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_search_record_set_group(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider search record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_search_record_set_description(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider search record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_search_record_hash(
    const UmiWorkbenchSelectionProviderSearchRecord *record);
/**
 * Provide the workbench selection provider search record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_search_record_touch(
    UmiWorkbenchSelectionProviderSearchRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
