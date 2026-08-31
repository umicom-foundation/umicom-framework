/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/social/test_community.c
 *
 * PURPOSE:
 *   Verify membership, posting and non-destructive moderation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/social/social.h"

/* Exercise community safety rules without a server or presentation toolkit. */
int main(void)
{
    UmiSocialCommunity *community = NULL;
    UmiSocialCommunitySnapshot snapshot;
    UmiSocialMessageRecord message;
    uint64_t message_id = 0U;

    /* An active member can create a copied message in a named channel. */
    assert(umi_social_community_create(4U, 8U, &community) == UMI_STATUS_OK);
    assert(umi_social_community_join(
        community, "sammy", "Sammy", UMI_SOCIAL_ADMINISTRATOR, 10U) ==
        UMI_STATUS_OK);
    assert(umi_social_community_post(
        community, "sammy", "general", "Welcome", 20U, &message_id) ==
        UMI_STATUS_OK);
    assert(umi_social_community_message_at(community, 0U, &message) ==
           UMI_STATUS_OK);
    assert(message.message_id == message_id);

    /* Suspension blocks new posts but preserves and can hide earlier evidence. */
    assert(umi_social_community_set_member_state(
        community, "sammy", UMI_SOCIAL_MEMBER_SUSPENDED) == UMI_STATUS_OK);
    assert(umi_social_community_post(
        community, "sammy", "general", "Blocked", 30U, &message_id) ==
        UMI_STATUS_PERMISSION_DENIED);
    assert(umi_social_community_set_message_hidden(
        community, 1U, true) == UMI_STATUS_OK);
    assert(umi_social_community_snapshot(community, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.suspended_members == 1U);
    assert(snapshot.messages == 1U);
    assert(snapshot.visible_messages == 0U);

    umi_social_community_destroy(community);
    return 0;
}
