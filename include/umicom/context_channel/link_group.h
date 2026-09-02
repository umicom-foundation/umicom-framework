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
/**
 * Represent the context link group data shared with callers of this public contract.
 */
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
/**
 * Represent the context link group store data shared with callers of this public contract.
 */
typedef struct UmiContextLinkGroupStore {
    UmiContextLinkGroup items[UMI_CONTEXT_LINK_GROUP_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextLinkGroupStore;
/**
 * Initialise context link group from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_link_group_init(UmiContextLinkGroup *record);
/**
 * Check that context link group satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_link_group_validate(const UmiContextLinkGroup *record);
/**
 * Initialise context link group store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_link_group_store_init(UmiContextLinkGroupStore *store);
/**
 * Provide the context link group store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_link_group_store_put(UmiContextLinkGroupStore *store,const UmiContextLinkGroup *record);
/**
 * Remove context link group store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_link_group_store_remove(UmiContextLinkGroupStore *store,const char *identity);
/**
 * Find context link group store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextLinkGroup *umi_context_link_group_store_find(UmiContextLinkGroupStore *store,const char *identity);
/**
 * Provide the context link group store find const operation used by this module and its
 * client applications.
 */
const UmiContextLinkGroup *umi_context_link_group_store_find_const(const UmiContextLinkGroupStore *store,const char *identity);
/**
 * Return the number of records represented by context link group store without changing
 * their state.
 */
size_t umi_context_link_group_store_count(const UmiContextLinkGroupStore *store);
/**
 * Provide the context link group store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_link_group_store_snapshot(const UmiContextLinkGroupStore *store,UmiContextLinkGroup *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
