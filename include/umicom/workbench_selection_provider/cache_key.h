/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/cache_key.h
 *
 * PURPOSE:
 *   Define the reusable provider cache key contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CACHE_KEY_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CACHE_KEY_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider cache key data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderCacheKey {
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
} UmiWorkbenchSelectionProviderCacheKey;

/**
 * Initialise workbench selection provider cache key from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_cache_key_init(
    UmiWorkbenchSelectionProviderCacheKey *record,
    const char *record_id);
/**
 * Check that workbench selection provider cache key satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_provider_cache_key_validate(
    const UmiWorkbenchSelectionProviderCacheKey *record);
/**
 * Provide the workbench selection provider cache key set provider operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_key_set_provider(
    UmiWorkbenchSelectionProviderCacheKey *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider cache key set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_key_set_source(
    UmiWorkbenchSelectionProviderCacheKey *record,
    const char *source_id);
/**
 * Provide the workbench selection provider cache key set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_key_set_subject(
    UmiWorkbenchSelectionProviderCacheKey *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider cache key set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_key_set_related(
    UmiWorkbenchSelectionProviderCacheKey *record,
    const char *related_id);
/**
 * Provide the workbench selection provider cache key set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_key_set_group(
    UmiWorkbenchSelectionProviderCacheKey *record,
    const char *group_id);
/**
 * Provide the workbench selection provider cache key set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_cache_key_set_description(
    UmiWorkbenchSelectionProviderCacheKey *record,
    const char *description);
/**
 * Provide the workbench selection provider cache key hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_cache_key_hash(
    const UmiWorkbenchSelectionProviderCacheKey *record);
/**
 * Provide the workbench selection provider cache key touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_provider_cache_key_touch(
    UmiWorkbenchSelectionProviderCacheKey *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
