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
typedef struct UmiRecentContextStore {
    UmiRecentContext items[UMI_CONTEXT_RECENT_CONTEXT_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiRecentContextStore;
void umi_context_recent_context_init(UmiRecentContext *record);
UmiStatus umi_context_recent_context_validate(const UmiRecentContext *record);
void umi_context_recent_context_store_init(UmiRecentContextStore *store);
UmiStatus umi_context_recent_context_store_put(UmiRecentContextStore *store,const UmiRecentContext *record);
UmiStatus umi_context_recent_context_store_remove(UmiRecentContextStore *store,const char *identity);
UmiRecentContext *umi_context_recent_context_store_find(UmiRecentContextStore *store,const char *identity);
const UmiRecentContext *umi_context_recent_context_store_find_const(const UmiRecentContextStore *store,const char *identity);
size_t umi_context_recent_context_store_count(const UmiRecentContextStore *store);
UmiStatus umi_context_recent_context_store_snapshot(const UmiRecentContextStore *store,UmiRecentContext *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
