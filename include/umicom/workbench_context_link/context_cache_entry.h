/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/context_cache_entry.h
 *
 * PURPOSE:
 *   Define the reusable context cache entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_CACHE_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_CACHE_ENTRY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link context cache entry data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextLinkContextCacheEntry {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkContextCacheEntry;

/**
 * Initialise workbench context link context cache entry from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_context_cache_entry_init(UmiWorkbenchContextLinkContextCacheEntry *record,
                                           const char *identity);
/**
 * Check that workbench context link context cache entry satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_context_cache_entry_validate(
    const UmiWorkbenchContextLinkContextCacheEntry *record);
/**
 * Copy workbench context link context cache entry into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_workbench_context_link_context_cache_entry_copy(
    UmiWorkbenchContextLinkContextCacheEntry *destination,
    const UmiWorkbenchContextLinkContextCacheEntry *source);
/**
 * Provide the workbench context link context cache entry hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_link_context_cache_entry_hash(
    const UmiWorkbenchContextLinkContextCacheEntry *record);
/**
 * Provide the workbench context link context cache entry set primary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_context_cache_entry_set_primary(
    UmiWorkbenchContextLinkContextCacheEntry *record,
    const char *value);
/**
 * Provide the workbench context link context cache entry set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_context_cache_entry_set_secondary(
    UmiWorkbenchContextLinkContextCacheEntry *record,
    const char *value);
/**
 * Provide the workbench context link context cache entry touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_link_context_cache_entry_touch(
    UmiWorkbenchContextLinkContextCacheEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
