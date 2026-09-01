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

UmiStatus umi_release_init(UmiRelease *release,
                           const UmiDeliveryManifest *manifest,
                           uint64_t generation)
{
    if (release == NULL || manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(release, 0, sizeof(*release));
    release->manifest = *manifest;
    release->generation = generation;
    return UMI_STATUS_OK;
}

int umi_release_ready_to_publish(const UmiRelease *release)
{
    return release != NULL &&
           release->verification == UMI_EVIDENCE_PASS &&
           release->published == 0;
}

void umi_release_mark_published(UmiRelease *release)
{
    if (release != NULL) release->published = 1;
}
