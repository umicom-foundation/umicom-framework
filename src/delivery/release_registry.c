/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_registry.c
 *
 * PURPOSE:
 *   Store a bounded history of release records and resolve releases by identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The registry gives update and rollback code one authoritative catalogue of known releases.
 */

#include "umicom/delivery/release_registry.h"
#include <string.h>

void umi_release_registry_init(UmiReleaseRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

UmiStatus umi_release_registry_add(UmiReleaseRegistry *registry,
                                   const UmiRelease *release)
{
    if (registry == NULL || release == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->releases[registry->count++] = *release;
    return UMI_STATUS_OK;
}

const UmiRelease *umi_release_registry_find(const UmiReleaseRegistry *registry,
                                            const char *release_id)
{
    size_t i;
    if (registry == NULL || release_id == NULL) return NULL;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->releases[i].manifest.release_id, release_id) == 0) {
            return &registry->releases[i];
        }
    }
    return NULL;
}

const UmiRelease *umi_release_registry_latest(const UmiReleaseRegistry *registry,
                                              UmiReleaseChannel channel)
{
    size_t i;
    const UmiRelease *latest = NULL;
    if (registry == NULL) return NULL;
    for (i = 0U; i < registry->count; ++i) {
        if (registry->releases[i].manifest.channel == channel &&
            (latest == NULL || registry->releases[i].generation > latest->generation)) {
            latest = &registry->releases[i];
        }
    }
    return latest;
}
