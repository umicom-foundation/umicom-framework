/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/social/community.h
 *
 * PURPOSE:
 *   Define a bounded social community model for membership, channels, message
 *   evidence and moderation in future Umicom community applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SOCIAL_COMMUNITY_H
#define UMICOM_SOCIAL_COMMUNITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOCIAL_ID_CAPACITY 64U
#define UMI_SOCIAL_NAME_CAPACITY 96U
#define UMI_SOCIAL_MESSAGE_CAPACITY 512U

/* Roles support least-privilege community policy without application classes. */
typedef enum UmiSocialMemberRole {
    UMI_SOCIAL_MEMBER = 0,
    UMI_SOCIAL_MODERATOR = 1,
    UMI_SOCIAL_ADMINISTRATOR = 2
} UmiSocialMemberRole;

/* Member state is explicit so suspension does not destroy audit identity. */
typedef enum UmiSocialMemberState {
    UMI_SOCIAL_MEMBER_ACTIVE = 0,
    UMI_SOCIAL_MEMBER_SUSPENDED = 1,
    UMI_SOCIAL_MEMBER_LEFT = 2
} UmiSocialMemberState;

/* Member snapshots copy identity and display data out of community storage. */
typedef struct UmiSocialMemberRecord {
    char member_id[UMI_SOCIAL_ID_CAPACITY];
    char display_name[UMI_SOCIAL_NAME_CAPACITY];
    UmiSocialMemberRole role;
    UmiSocialMemberState state;
    uint64_t joined_at_ms;
} UmiSocialMemberRecord;

/* Messages retain immutable author, channel, timestamp and bounded content. */
typedef struct UmiSocialMessageRecord {
    uint64_t message_id;
    char member_id[UMI_SOCIAL_ID_CAPACITY];
    char channel_id[UMI_SOCIAL_ID_CAPACITY];
    char content[UMI_SOCIAL_MESSAGE_CAPACITY];
    uint64_t created_at_ms;
    bool hidden;
} UmiSocialMessageRecord;

/* The opaque community owns all member and message records. */
typedef struct UmiSocialCommunity UmiSocialCommunity;

/* Community snapshots provide UI counts without exposing owned arrays. */
typedef struct UmiSocialCommunitySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t active_members;
    size_t suspended_members;
    size_t messages;
    size_t visible_messages;
    uint64_t revision;
} UmiSocialCommunitySnapshot;

/* Create a community with explicit member and message limits. */
UmiStatus umi_social_community_create(
    size_t member_capacity,
    size_t message_capacity,
    UmiSocialCommunity **out_community);

/* Destroy every member and message record owned by the community. */
void umi_social_community_destroy(UmiSocialCommunity *community);

/* Join a unique member with copied identity and display text. */
UmiStatus umi_social_community_join(
    UmiSocialCommunity *community,
    const char *member_id,
    const char *display_name,
    UmiSocialMemberRole role,
    uint64_t joined_at_ms);

/* Mark an existing member as left without erasing historical authorship. */
UmiStatus umi_social_community_leave(
    UmiSocialCommunity *community,
    const char *member_id);

/* Suspend or restore a member while retaining their immutable identity. */
UmiStatus umi_social_community_set_member_state(
    UmiSocialCommunity *community,
    const char *member_id,
    UmiSocialMemberState state);

/* Append an immutable message from an active member to a named channel. */
UmiStatus umi_social_community_post(
    UmiSocialCommunity *community,
    const char *member_id,
    const char *channel_id,
    const char *content,
    uint64_t created_at_ms,
    uint64_t *out_message_id);

/* Hide or restore a message without deleting moderation evidence. */
UmiStatus umi_social_community_set_message_hidden(
    UmiSocialCommunity *community,
    uint64_t message_id,
    bool hidden);

/* Copy one member by stable identity. */
UmiStatus umi_social_community_find_member(
    const UmiSocialCommunity *community,
    const char *member_id,
    UmiSocialMemberRecord *out_member);

/* Copy one message by chronological insertion index. */
UmiStatus umi_social_community_message_at(
    const UmiSocialCommunity *community,
    size_t index,
    UmiSocialMessageRecord *out_message);

/* Copy community counts and revision for panels and policy checks. */
UmiStatus umi_social_community_snapshot(
    const UmiSocialCommunity *community,
    UmiSocialCommunitySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
