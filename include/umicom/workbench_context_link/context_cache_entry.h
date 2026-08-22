/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/context_cache_entry.h
 *
 * PURPOSE:
 *   Define the reusable context cache entry contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_CACHE_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONTEXT_CACHE_ENTRY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_context_cache_entry_init(UmiWorkbenchContextLinkContextCacheEntry *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_context_cache_entry_validate(
    const UmiWorkbenchContextLinkContextCacheEntry *record);
UmiStatus umi_workbench_context_link_context_cache_entry_copy(
    UmiWorkbenchContextLinkContextCacheEntry *destination,
    const UmiWorkbenchContextLinkContextCacheEntry *source);
uint64_t umi_workbench_context_link_context_cache_entry_hash(
    const UmiWorkbenchContextLinkContextCacheEntry *record);
UmiStatus umi_workbench_context_link_context_cache_entry_set_primary(
    UmiWorkbenchContextLinkContextCacheEntry *record,
    const char *value);
UmiStatus umi_workbench_context_link_context_cache_entry_set_secondary(
    UmiWorkbenchContextLinkContextCacheEntry *record,
    const char *value);
void umi_workbench_context_link_context_cache_entry_touch(
    UmiWorkbenchContextLinkContextCacheEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
