/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/update_channel_v2.h
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
#ifndef UMICOM_DELIVERY_UPDATE_CHANNEL_V2_H
#define UMICOM_DELIVERY_UPDATE_CHANNEL_V2_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUpdateChannelV2 {
    char channel_id[UMI_DELIVERY_ID_CAPACITY];
    char feed_url[UMI_DELIVERY_PATH_CAPACITY];
    UmiReleaseChannel channel;
    unsigned rollout_percentage;
    int allow_prerelease;
    int require_signature;
} UmiUpdateChannelV2;

UmiStatus umi_update_channel_v2_init(UmiUpdateChannelV2 *channel,
                                      const char *channel_id,
                                      const char *feed_url,
                                      UmiReleaseChannel release_channel,
                                      unsigned rollout_percentage);
UmiStatus umi_update_channel_v2_validate(const UmiUpdateChannelV2 *channel);
int umi_update_channel_v2_offers(const UmiUpdateChannelV2 *channel,
                                 UmiReleaseChannel release_channel,
                                 unsigned cohort);

#ifdef __cplusplus
}
#endif
#endif
