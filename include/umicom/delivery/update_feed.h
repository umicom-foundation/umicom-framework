/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/update_feed.h
 *
 * PURPOSE:
 *   Maintain a bounded feed of releases available to update clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The feed separates release discovery from update installation and can later be backed by GitHub or another registry.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_UPDATE_FEED_H
#define INCLUDE_UMICOM_DELIVERY_UPDATE_FEED_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/release.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the update feed data shared with callers of this public contract.
 */
typedef struct UmiUpdateFeed {
    UmiRelease releases[UMI_DELIVERY_MAX_RELEASES];
    size_t count;
} UmiUpdateFeed;

/**
 * Initialise update feed from caller-provided values so later operations receive a known
 * state.
 */
void umi_update_feed_init(UmiUpdateFeed *feed);
/**
 * Add update feed only after its inputs and available capacity have been checked.
 */
UmiStatus umi_update_feed_add(UmiUpdateFeed *feed,
                              const UmiRelease *release);
/**
 * Provide the update feed latest operation used by this module and its client
 * applications.
 */
const UmiRelease *umi_update_feed_latest(const UmiUpdateFeed *feed,
                                         UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif

#endif
