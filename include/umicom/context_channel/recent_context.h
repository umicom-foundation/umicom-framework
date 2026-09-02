/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/recent_context.h
 *
 * PURPOSE:
 *   Track recent and pinned context subjects for navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_RECENT_CONTEXT_H
#define UMICOM_CONTEXT_CHANNEL_RECENT_CONTEXT_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_RECENT_CONTEXT_MAX_ITEMS 128U
/**
 * Represent the recent context data shared with callers of this public contract.
 */
typedef struct UmiRecentContext {
    uint32_t structure_size;
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char label[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t sequence;
    uint64_t last_used_ms;
    bool pinned;
    uint64_t revision;
} UmiRecentContext;
/**
 * Represent the recent context store data shared with callers of this public contract.
 */
typedef struct UmiRecentContextStore {
    UmiRecentContext items[UMI_CONTEXT_RECENT_CONTEXT_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiRecentContextStore;
/**
 * Initialise context recent context from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_recent_context_init(UmiRecentContext *record);
/**
 * Check that context recent context satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_recent_context_validate(const UmiRecentContext *record);
/**
 * Initialise context recent context store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_recent_context_store_init(UmiRecentContextStore *store);
/**
 * Provide the context recent context store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_recent_context_store_put(UmiRecentContextStore *store,const UmiRecentContext *record);
/**
 * Remove context recent context store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_recent_context_store_remove(UmiRecentContextStore *store,const char *identity);
/**
 * Find context recent context store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiRecentContext *umi_context_recent_context_store_find(UmiRecentContextStore *store,const char *identity);
/**
 * Provide the context recent context store find const operation used by this module and
 * its client applications.
 */
const UmiRecentContext *umi_context_recent_context_store_find_const(const UmiRecentContextStore *store,const char *identity);
/**
 * Return the number of records represented by context recent context store without
 * changing their state.
 */
size_t umi_context_recent_context_store_count(const UmiRecentContextStore *store);
/**
 * Provide the context recent context store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_recent_context_store_snapshot(const UmiRecentContextStore *store,UmiRecentContext *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
