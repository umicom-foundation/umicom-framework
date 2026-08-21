/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/channel.h
 *
 * PURPOSE:
 *   Define named typed channels and their presentation colour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CHANNEL_H
#define UMICOM_CONTEXT_CHANNEL_CHANNEL_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_CHANNEL_MAX_ITEMS 32U
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
typedef struct UmiContextChannelStore {
    UmiContextChannel items[UMI_CONTEXT_CHANNEL_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextChannelStore;
void umi_context_channel_init(UmiContextChannel *record);
UmiStatus umi_context_channel_validate(const UmiContextChannel *record);
void umi_context_channel_store_init(UmiContextChannelStore *store);
UmiStatus umi_context_channel_store_put(UmiContextChannelStore *store,const UmiContextChannel *record);
UmiStatus umi_context_channel_store_remove(UmiContextChannelStore *store,const char *identity);
UmiContextChannel *umi_context_channel_store_find(UmiContextChannelStore *store,const char *identity);
const UmiContextChannel *umi_context_channel_store_find_const(const UmiContextChannelStore *store,const char *identity);
size_t umi_context_channel_store_count(const UmiContextChannelStore *store);
UmiStatus umi_context_channel_store_snapshot(const UmiContextChannelStore *store,UmiContextChannel *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
