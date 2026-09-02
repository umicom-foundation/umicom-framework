/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/channel.h
 *
 * PURPOSE:
 *   Define named typed channels and their presentation colour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CHANNEL_H
#define UMICOM_CONTEXT_CHANNEL_CHANNEL_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_CHANNEL_MAX_ITEMS 32U
/**
 * Represent the context channel data shared with callers of this public contract.
 */
typedef struct UmiContextChannel {
    uint32_t structure_size;
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char display_name[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    UmiContextKind kind;
    UmiContextChannelColour colour;
    bool enabled;
    bool persistent;
    uint64_t last_sequence;
    uint64_t revision;
} UmiContextChannel;
/**
 * Represent the context channel store data shared with callers of this public contract.
 */
typedef struct UmiContextChannelStore {
    UmiContextChannel items[UMI_CONTEXT_CHANNEL_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextChannelStore;
/**
 * Initialise context channel from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_channel_init(UmiContextChannel *record);
/**
 * Check that context channel satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_channel_validate(const UmiContextChannel *record);
/**
 * Initialise context channel store from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_channel_store_init(UmiContextChannelStore *store);
/**
 * Provide the context channel store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_channel_store_put(UmiContextChannelStore *store,const UmiContextChannel *record);
/**
 * Remove context channel store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_channel_store_remove(UmiContextChannelStore *store,const char *identity);
/**
 * Find context channel store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextChannel *umi_context_channel_store_find(UmiContextChannelStore *store,const char *identity);
/**
 * Provide the context channel store find const operation used by this module and its
 * client applications.
 */
const UmiContextChannel *umi_context_channel_store_find_const(const UmiContextChannelStore *store,const char *identity);
/**
 * Return the number of records represented by context channel store without changing their
 * state.
 */
size_t umi_context_channel_store_count(const UmiContextChannelStore *store);
/**
 * Provide the context channel store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_channel_store_snapshot(const UmiContextChannelStore *store,UmiContextChannel *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
