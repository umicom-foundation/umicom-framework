/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/cache_record.h
 *
 * PURPOSE:
 *   Define the reusable provider cache record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CACHE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CACHE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider cache record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderCacheRecord {
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
} UmiWorkbenchSelectionProviderCacheRecord;

/**
 * Initialise workbench selection provider cache record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_cache_record_init(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider cache record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_cache_record_validate(
    const UmiWorkbenchSelectionProviderCacheRecord *record);
/**
 * Provide the workbench selection provider cache record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_record_set_provider(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider cache record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_record_set_source(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider cache record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_record_set_subject(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider cache record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_record_set_related(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider cache record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_record_set_group(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider cache record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_record_set_description(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider cache record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_cache_record_hash(
    const UmiWorkbenchSelectionProviderCacheRecord *record);
/**
 * Provide the workbench selection provider cache record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_cache_record_touch(
    UmiWorkbenchSelectionProviderCacheRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
