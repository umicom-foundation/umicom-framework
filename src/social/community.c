/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/social/community.c
 *
 * PURPOSE:
 *   Implement bounded copied community records with non-destructive moderation
 *   so history never contains dangling member or message references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/social/community.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Community ownership keeps both arrays and identity allocation together. */
struct UmiSocialCommunity {
    UmiSocialMemberRecord *members;
    UmiSocialMessageRecord *messages;
    size_t member_capacity;
    size_t member_count;
    size_t message_capacity;
    size_t message_count;
    uint64_t next_message_id;
    uint64_t revision;
};

/* Copy externally supplied text and reject truncation instead of changing IDs. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *text)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", text);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Find mutable membership state internally without exporting an owned pointer. */
static UmiSocialMemberRecord *find_member_mutable(
    UmiSocialCommunity *community,
    const char *member_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || member_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < community->member_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(community->members[index].member_id, member_id) == 0) {
            return &community->members[index];
        }
    }
    return NULL;
}

/* Find mutable message moderation state by stable numeric identity. */
static UmiSocialMessageRecord *find_message_mutable(
    UmiSocialCommunity *community,
    uint64_t message_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || message_id == 0U) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < community->message_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (community->messages[index].message_id == message_id) {
            return &community->messages[index];
        }
    }
    return NULL;
}

/* Allocate both checked bounded arrays or leave the caller with no community. */
UmiStatus umi_social_community_create(
    size_t member_capacity,
    size_t message_capacity,
    UmiSocialCommunity **out_community)
{
    UmiSocialCommunity *community;
    /* Both multiplication operations are checked before allocation. */
    if (out_community == NULL || member_capacity == 0U ||
        message_capacity == 0U ||
        member_capacity > SIZE_MAX / sizeof(UmiSocialMemberRecord) ||
        message_capacity > SIZE_MAX / sizeof(UmiSocialMessageRecord)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_community = NULL;
    community = (UmiSocialCommunity *)calloc(1U, sizeof(*community));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    community->members = (UmiSocialMemberRecord *)calloc(
        member_capacity, sizeof(*community->members));
    community->messages = (UmiSocialMessageRecord *)calloc(
        message_capacity, sizeof(*community->messages));
    /* A community is useful only when both owned stores were allocated. */
    if (community->members == NULL || community->messages == NULL) {
        free(community->messages);
        free(community->members);
        free(community);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    community->member_capacity = member_capacity;
    community->message_capacity = message_capacity;
    community->next_message_id = 1U;
    community->revision = 1U;
    *out_community = community;
    return UMI_STATUS_OK;
}

/* Release message and member storage before their common community owner. */
void umi_social_community_destroy(UmiSocialCommunity *community)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL) return;
    free(community->messages);
    free(community->members);
    free(community);
}

/* Add one durable member identity with exact copied display text. */
UmiStatus umi_social_community_join(
    UmiSocialCommunity *community,
    const char *member_id,
    const char *display_name,
    UmiSocialMemberRole role,
    uint64_t joined_at_ms)
{
    UmiSocialMemberRecord *member;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || member_id == NULL || member_id[0] == '\0' ||
        display_name == NULL || display_name[0] == '\0' ||
        role < UMI_SOCIAL_MEMBER || role > UMI_SOCIAL_ADMINISTRATOR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Reusing an identity would make historical authorship ambiguous. */
    if (find_member_mutable(community, member_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (community->member_count >= community->member_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    member = &community->members[community->member_count];
    (void)memset(member, 0, sizeof(*member));
    status = copy_text(member->member_id, sizeof(member->member_id), member_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = copy_text(
        member->display_name, sizeof(member->display_name), display_name);
    /* The member is counted only after all text has copied without truncation. */
    if (status != UMI_STATUS_OK) return status;
    member->role = role;
    member->state = UMI_SOCIAL_MEMBER_ACTIVE;
    member->joined_at_ms = joined_at_ms;
    community->member_count += 1U;
    community->revision += 1U;
    return UMI_STATUS_OK;
}

/* Preserve member identity but prevent future posts by marking the member left. */
UmiStatus umi_social_community_leave(
    UmiSocialCommunity *community,
    const char *member_id)
{
    UmiSocialMemberRecord *member;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || member_id == NULL || member_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    member = find_member_mutable(community, member_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (member == NULL) return UMI_STATUS_NOT_FOUND;
    member->state = UMI_SOCIAL_MEMBER_LEFT;
    community->revision += 1U;
    return UMI_STATUS_OK;
}

/* Apply an explicit moderation state while retaining all historical records. */
UmiStatus umi_social_community_set_member_state(
    UmiSocialCommunity *community,
    const char *member_id,
    UmiSocialMemberState state)
{
    UmiSocialMemberRecord *member;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || member_id == NULL || member_id[0] == '\0' ||
        state < UMI_SOCIAL_MEMBER_ACTIVE || state > UMI_SOCIAL_MEMBER_LEFT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    member = find_member_mutable(community, member_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (member == NULL) return UMI_STATUS_NOT_FOUND;
    member->state = state;
    community->revision += 1U;
    return UMI_STATUS_OK;
}

/* Append a message only when its exact author identity is currently active. */
UmiStatus umi_social_community_post(
    UmiSocialCommunity *community,
    const char *member_id,
    const char *channel_id,
    const char *content,
    uint64_t created_at_ms,
    uint64_t *out_message_id)
{
    UmiSocialMemberRecord *member;
    UmiSocialMessageRecord *message;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || member_id == NULL || member_id[0] == '\0' ||
        channel_id == NULL || channel_id[0] == '\0' || content == NULL ||
        content[0] == '\0' || out_message_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    member = find_member_mutable(community, member_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (member == NULL) return UMI_STATUS_NOT_FOUND;
    /* Suspended and departed members retain history but cannot create messages. */
    if (member->state != UMI_SOCIAL_MEMBER_ACTIVE) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (community->message_count >= community->message_capacity ||
        community->next_message_id == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    message = &community->messages[community->message_count];
    (void)memset(message, 0, sizeof(*message));
    status = copy_text(message->member_id,
                       sizeof(message->member_id), member_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = copy_text(
        message->channel_id, sizeof(message->channel_id), channel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = copy_text(
        message->content, sizeof(message->content), content);
    /* A partially copied message is never assigned an identity or counted. */
    if (status != UMI_STATUS_OK) return status;
    message->message_id = community->next_message_id;
    message->created_at_ms = created_at_ms;
    *out_message_id = message->message_id;
    community->next_message_id += 1U;
    community->message_count += 1U;
    community->revision += 1U;
    return UMI_STATUS_OK;
}

/* Toggle visibility without destroying the message required for audit evidence. */
UmiStatus umi_social_community_set_message_hidden(
    UmiSocialCommunity *community,
    uint64_t message_id,
    bool hidden)
{
    UmiSocialMessageRecord *message;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || message_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    message = find_message_mutable(community, message_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (message == NULL) return UMI_STATUS_NOT_FOUND;
    message->hidden = hidden;
    community->revision += 1U;
    return UMI_STATUS_OK;
}

/* Copy a member by identity so no caller retains a community-owned pointer. */
UmiStatus umi_social_community_find_member(
    const UmiSocialCommunity *community,
    const char *member_id,
    UmiSocialMemberRecord *out_member)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || member_id == NULL || member_id[0] == '\0' ||
        out_member == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < community->member_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(community->members[index].member_id, member_id) == 0) {
            *out_member = community->members[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Copy a chronological message record by its bounded insertion index. */
UmiStatus umi_social_community_message_at(
    const UmiSocialCommunity *community,
    size_t index,
    UmiSocialMessageRecord *out_message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || out_message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= community->message_count) return UMI_STATUS_NOT_FOUND;
    *out_message = community->messages[index];
    return UMI_STATUS_OK;
}

/* Count visible, active and suspended records for UI and moderation dashboards. */
UmiStatus umi_social_community_snapshot(
    const UmiSocialCommunity *community,
    UmiSocialCommunitySnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (community == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->messages = community->message_count;
    out_snapshot->revision = community->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < community->member_count; ++index) {
        /* Left members remain historical records but not current member counts. */
        if (community->members[index].state == UMI_SOCIAL_MEMBER_ACTIVE) {
            out_snapshot->active_members += 1U;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (community->members[index].state ==
                   UMI_SOCIAL_MEMBER_SUSPENDED) {
            out_snapshot->suspended_members += 1U;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < community->message_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!community->messages[index].hidden) {
            out_snapshot->visible_messages += 1U;
        }
    }
    return UMI_STATUS_OK;
}
