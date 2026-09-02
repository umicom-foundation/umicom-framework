/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/update_feed.c
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

#include "umicom/delivery/update_feed.h"
#include <string.h>

/*
 * Initialise update feed from caller-provided values so later operations receive a known
 * state.
 */
void umi_update_feed_init(UmiUpdateFeed *feed)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (feed != NULL) (void)memset(feed, 0, sizeof(*feed));
}

/* Add update feed only after its inputs and available capacity have been checked. */
UmiStatus umi_update_feed_add(UmiUpdateFeed *feed,
                              const UmiRelease *release)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (feed == NULL || release == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (feed->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    feed->releases[feed->count++] = *release;
    return UMI_STATUS_OK;
}

/*
 * Provide the update feed latest operation used by this module and its client
 * applications.
 */
const UmiRelease *umi_update_feed_latest(const UmiUpdateFeed *feed,
                                         UmiReleaseChannel channel)
{
    size_t i;
    const UmiRelease *latest = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (feed == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < feed->count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (feed->releases[i].manifest.channel == channel &&
            (latest == NULL || feed->releases[i].generation > latest->generation)) {
            latest = &feed->releases[i];
        }
    }
    return latest;
}
