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
typedef struct UmiContextGroupMemberStore {
    UmiContextGroupMember items[UMI_CONTEXT_GROUP_MEMBER_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextGroupMemberStore;
void umi_context_group_member_init(UmiContextGroupMember *record);
UmiStatus umi_context_group_member_validate(const UmiContextGroupMember *record);
void umi_context_group_member_store_init(UmiContextGroupMemberStore *store);
UmiStatus umi_context_group_member_store_put(UmiContextGroupMemberStore *store,const UmiContextGroupMember *record);
UmiStatus umi_context_group_member_store_remove(UmiContextGroupMemberStore *store,const char *identity);
UmiContextGroupMember *umi_context_group_member_store_find(UmiContextGroupMemberStore *store,const char *identity);
const UmiContextGroupMember *umi_context_group_member_store_find_const(const UmiContextGroupMemberStore *store,const char *identity);
size_t umi_context_group_member_store_count(const UmiContextGroupMemberStore *store);
UmiStatus umi_context_group_member_store_snapshot(const UmiContextGroupMemberStore *store,UmiContextGroupMember *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
