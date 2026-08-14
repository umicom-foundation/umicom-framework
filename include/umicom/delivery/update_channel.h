/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/update_channel.h
 *
 * PURPOSE:
 *   Control update feed endpoints, release channels and staged rollout cohorts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_UPDATE_CHANNEL_H
#define UMICOM_DELIVERY_UPDATE_CHANNEL_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUpdateChannel {
    char channel_id[UMI_DELIVERY_ID_CAPACITY];
    char feed_url[UMI_DELIVERY_PATH_CAPACITY];
    UmiReleaseChannel channel;
    unsigned rollout_percentage;
    int allow_prerelease;
    int require_signature;
} UmiUpdateChannel;

UmiStatus umi_update_channel_init(UmiUpdateChannel *channel,
                                      const char *channel_id,
                                      const char *feed_url,
                                      UmiReleaseChannel release_channel,
                                      unsigned rollout_percentage);
UmiStatus umi_update_channel_validate(const UmiUpdateChannel *channel);
int umi_update_channel_offers(const UmiUpdateChannel *channel,
                                 UmiReleaseChannel release_channel,
                                 unsigned cohort);

#ifdef __cplusplus
}
#endif
#endif
