/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/group_member.h
 *
 * PURPOSE:
 *   Track members of coloured typed context groups.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_GROUP_MEMBER_H
#define UMICOM_CONTEXT_CHANNEL_GROUP_MEMBER_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_GROUP_MEMBER_MAX_ITEMS 256U
/**
 * Represent the context group member data shared with callers of this public contract.
 */
typedef struct UmiContextGroupMember {
    uint32_t structure_size;
    char member_id[UMI_CONTEXT_TEXT_CAPACITY];
    char group_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char panel_instance_id[UMI_CONTEXT_TEXT_CAPACITY];
    UmiContextSubscriptionRole role;
    bool enabled;
    uint64_t revision;
} UmiContextGroupMember;
/**
 * Represent the context group member store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextGroupMemberStore {
    UmiContextGroupMember items[UMI_CONTEXT_GROUP_MEMBER_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextGroupMemberStore;
/**
 * Initialise context group member from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_group_member_init(UmiContextGroupMember *record);
/**
 * Check that context group member satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_group_member_validate(const UmiContextGroupMember *record);
/**
 * Initialise context group member store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_group_member_store_init(UmiContextGroupMemberStore *store);
/**
 * Provide the context group member store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_group_member_store_put(UmiContextGroupMemberStore *store,const UmiContextGroupMember *record);
/**
 * Remove context group member store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_group_member_store_remove(UmiContextGroupMemberStore *store,const char *identity);
/**
 * Find context group member store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextGroupMember *umi_context_group_member_store_find(UmiContextGroupMemberStore *store,const char *identity);
/**
 * Provide the context group member store find const operation used by this module and its
 * client applications.
 */
const UmiContextGroupMember *umi_context_group_member_store_find_const(const UmiContextGroupMemberStore *store,const char *identity);
/**
 * Return the number of records represented by context group member store without changing
 * their state.
 */
size_t umi_context_group_member_store_count(const UmiContextGroupMemberStore *store);
/**
 * Provide the context group member store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_group_member_store_snapshot(const UmiContextGroupMemberStore *store,UmiContextGroupMember *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
