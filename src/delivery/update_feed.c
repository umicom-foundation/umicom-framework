/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/update_feed.c
 *
 * PURPOSE:
 *   Maintain a bounded feed of releases available to update clients.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The feed separates release discovery from update installation and can later be backed by GitHub or another registry.
 */

#include "umicom/delivery/update_feed.h"
#include <string.h>

void umi_update_feed_init(UmiUpdateFeed *feed)
{
    if (feed != NULL) (void)memset(feed, 0, sizeof(*feed));
}

UmiStatus umi_update_feed_add(UmiUpdateFeed *feed,
                              const UmiRelease *release)
{
    if (feed == NULL || release == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (feed->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    feed->releases[feed->count++] = *release;
    return UMI_STATUS_OK;
}

const UmiRelease *umi_update_feed_latest(const UmiUpdateFeed *feed,
                                         UmiReleaseChannel channel)
{
    size_t i;
    const UmiRelease *latest = NULL;
    if (feed == NULL) return NULL;
    for (i = 0U; i < feed->count; ++i) {
        if (feed->releases[i].manifest.channel == channel &&
            (latest == NULL || feed->releases[i].generation > latest->generation)) {
            latest = &feed->releases[i];
        }
    }
    return latest;
}
