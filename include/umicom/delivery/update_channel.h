/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/update_channel.h
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
#ifndef UMICOM_DELIVERY_UPDATE_CHANNEL_H
#define UMICOM_DELIVERY_UPDATE_CHANNEL_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the update channel data shared with callers of this public contract.
 */
typedef struct UmiUpdateChannel {
    char channel_id[UMI_DELIVERY_ID_CAPACITY];
    char feed_url[UMI_DELIVERY_PATH_CAPACITY];
    UmiReleaseChannel channel;
    unsigned rollout_percentage;
    int allow_prerelease;
    int require_signature;
} UmiUpdateChannel;

/**
 * Initialise update channel from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_update_channel_init(UmiUpdateChannel *channel,
                                      const char *channel_id,
                                      const char *feed_url,
                                      UmiReleaseChannel release_channel,
                                      unsigned rollout_percentage);
/**
 * Check that update channel satisfies its contract before another service relies on it.
 */
UmiStatus umi_update_channel_validate(const UmiUpdateChannel *channel);
/**
 * Provide the update channel offers operation used by this module and its client
 * applications.
 */
int umi_update_channel_offers(const UmiUpdateChannel *channel,
                                 UmiReleaseChannel release_channel,
                                 unsigned cohort);

#ifdef __cplusplus
}
#endif
#endif
