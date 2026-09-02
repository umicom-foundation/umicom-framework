/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_registry.c
 *
 * PURPOSE:
 *   Store a bounded history of release records and resolve releases by identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The registry gives update and rollback code one authoritative catalogue of known releases.
 */

#include "umicom/delivery/release_registry.h"
#include <string.h>

/*
 * Initialise release registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_release_registry_init(UmiReleaseRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

/* Add release registry only after its inputs and available capacity have been checked. */
UmiStatus umi_release_registry_add(UmiReleaseRegistry *registry,
                                   const UmiRelease *release)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || release == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->releases[registry->count++] = *release;
    return UMI_STATUS_OK;
}

/*
 * Find release registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiRelease *umi_release_registry_find(const UmiReleaseRegistry *registry,
                                            const char *release_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || release_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->releases[i].manifest.release_id, release_id) == 0) {
            return &registry->releases[i];
        }
    }
    return NULL;
}

/*
 * Provide the release registry latest operation used by this module and its client
 * applications.
 */
const UmiRelease *umi_release_registry_latest(const UmiReleaseRegistry *registry,
                                              UmiReleaseChannel channel)
{
    size_t i;
    const UmiRelease *latest = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (registry->releases[i].manifest.channel == channel &&
            (latest == NULL || registry->releases[i].generation > latest->generation)) {
            latest = &registry->releases[i];
        }
    }
    return latest;
}
