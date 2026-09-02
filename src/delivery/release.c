/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release.c
 *
 * PURPOSE:
 *   Implement one immutable release candidate and its promotion state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A release combines manifest identity, channel, package evidence and promotion status into one inspectable record.
 */

#include "umicom/delivery/release.h"
#include <string.h>

/*
 * Initialise release from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_release_init(UmiRelease *release,
                           const UmiDeliveryManifest *manifest,
                           uint64_t generation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (release == NULL || manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(release, 0, sizeof(*release));
    release->manifest = *manifest;
    release->generation = generation;
    return UMI_STATUS_OK;
}

/*
 * Provide the release ready to publish operation used by this module and its client
 * applications.
 */
int umi_release_ready_to_publish(const UmiRelease *release)
{
    return release != NULL &&
           release->verification == UMI_EVIDENCE_PASS &&
           release->published == 0;
}

/*
 * Provide the release mark published operation used by this module and its client
 * applications.
 */
void umi_release_mark_published(UmiRelease *release)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (release != NULL) release->published = 1;
}
