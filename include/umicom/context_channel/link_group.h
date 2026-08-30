/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/link_group.h
 *
 * PURPOSE:
 *   Represent coloured window groups as typed Framework identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_LINK_GROUP_H
#define UMICOM_CONTEXT_CHANNEL_LINK_GROUP_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_LINK_GROUP_MAX_ITEMS 32U
typedef struct UmiContextLinkGroup {
    uint32_t structure_size;
    char group_id[UMI_CONTEXT_TEXT_CAPACITY];
    char display_name[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    UmiContextKind kind;
    UmiContextChannelColour colour;
    bool locked;
    bool enabled;
    uint64_t revision;
} UmiContextLinkGroup;
typedef struct UmiContextLinkGroupStore {
    UmiContextLinkGroup items[UMI_CONTEXT_LINK_GROUP_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextLinkGroupStore;
void umi_context_link_group_init(UmiContextLinkGroup *record);
UmiStatus umi_context_link_group_validate(const UmiContextLinkGroup *record);
void umi_context_link_group_store_init(UmiContextLinkGroupStore *store);
UmiStatus umi_context_link_group_store_put(UmiContextLinkGroupStore *store,const UmiContextLinkGroup *record);
UmiStatus umi_context_link_group_store_remove(UmiContextLinkGroupStore *store,const char *identity);
UmiContextLinkGroup *umi_context_link_group_store_find(UmiContextLinkGroupStore *store,const char *identity);
const UmiContextLinkGroup *umi_context_link_group_store_find_const(const UmiContextLinkGroupStore *store,const char *identity);
size_t umi_context_link_group_store_count(const UmiContextLinkGroupStore *store);
UmiStatus umi_context_link_group_store_snapshot(const UmiContextLinkGroupStore *store,UmiContextLinkGroup *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
