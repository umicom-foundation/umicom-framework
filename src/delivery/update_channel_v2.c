/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/update_channel_v2.c
 *
 * PURPOSE:
 *   Control update feed endpoints, release channels and staged rollout cohorts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/update_channel_v2.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_update_channel_v2_init(UmiUpdateChannelV2 *channel,
                                      const char *channel_id,
                                      const char *feed_url,
                                      UmiReleaseChannel release_channel,
                                      unsigned rollout_percentage)
{
    UmiStatus status;
    if (channel == NULL || channel_id == NULL || feed_url == NULL ||
        rollout_percentage > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(channel, 0, sizeof(*channel));
    status = umi_delivery_copy_text(channel->channel_id,
                                    sizeof(channel->channel_id), channel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(channel->feed_url,
                                    sizeof(channel->feed_url), feed_url);
    if (status != UMI_STATUS_OK) return status;
    channel->channel = release_channel;
    channel->rollout_percentage = rollout_percentage;
    channel->allow_prerelease = release_channel != UMI_RELEASE_STABLE;
    channel->require_signature = release_channel == UMI_RELEASE_STABLE;
    return UMI_STATUS_OK;
}

UmiStatus umi_update_channel_v2_validate(const UmiUpdateChannelV2 *channel)
{
    if (channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (channel->channel_id[0] == '\0' || channel->feed_url[0] == '\0' ||
        channel->channel < UMI_RELEASE_DEVELOPMENT ||
        channel->channel > UMI_RELEASE_STABLE ||
        channel->rollout_percentage > 100U) return UMI_STATUS_INVALID_STATE;
    if (channel->channel == UMI_RELEASE_STABLE &&
        !channel->require_signature) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

int umi_update_channel_v2_offers(const UmiUpdateChannelV2 *channel,
                                 UmiReleaseChannel release_channel,
                                 unsigned cohort)
{
    if (umi_update_channel_v2_validate(channel) != UMI_STATUS_OK ||
        cohort >= 100U || release_channel != channel->channel ||
        cohort >= channel->rollout_percentage) return 0;
    if (release_channel != UMI_RELEASE_STABLE && !channel->allow_prerelease) {
        return 0;
    }
    return 1;
}
