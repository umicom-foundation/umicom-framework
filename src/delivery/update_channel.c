/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/update_channel.c
 *
 * PURPOSE:
 *   Control update feed endpoints, release channels and staged rollout cohorts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/update_channel.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise update channel from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_update_channel_init(UmiUpdateChannel *channel,
                                      const char *channel_id,
                                      const char *feed_url,
                                      UmiReleaseChannel release_channel,
                                      unsigned rollout_percentage)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (channel == NULL || channel_id == NULL || feed_url == NULL ||
        rollout_percentage > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(channel, 0, sizeof(*channel));
    status = umi_delivery_copy_text(channel->channel_id,
                                    sizeof(channel->channel_id), channel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(channel->feed_url,
                                    sizeof(channel->feed_url), feed_url);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    channel->channel = release_channel;
    channel->rollout_percentage = rollout_percentage;
    channel->allow_prerelease = release_channel != UMI_RELEASE_STABLE;
    channel->require_signature = release_channel == UMI_RELEASE_STABLE;
    return UMI_STATUS_OK;
}

/* Check that update channel satisfies its contract before another service relies on it. */
UmiStatus umi_update_channel_validate(const UmiUpdateChannel *channel)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (channel->channel_id[0] == '\0' || channel->feed_url[0] == '\0' ||
        channel->channel < UMI_RELEASE_DEVELOPMENT ||
        channel->channel > UMI_RELEASE_STABLE ||
        channel->rollout_percentage > 100U) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (channel->channel == UMI_RELEASE_STABLE &&
        !channel->require_signature) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the update channel offers operation used by this module and its client
 * applications.
 */
int umi_update_channel_offers(const UmiUpdateChannel *channel,
                                 UmiReleaseChannel release_channel,
                                 unsigned cohort)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_update_channel_validate(channel) != UMI_STATUS_OK ||
        cohort >= 100U || release_channel != channel->channel ||
        cohort >= channel->rollout_percentage) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (release_channel != UMI_RELEASE_STABLE && !channel->allow_prerelease) {
        return 0;
    }
    return 1;
}
