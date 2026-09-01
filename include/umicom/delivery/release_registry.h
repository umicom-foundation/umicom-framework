/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_registry.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_RELEASE_REGISTRY_H
#define INCLUDE_UMICOM_DELIVERY_RELEASE_REGISTRY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/release.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseRegistry {
    UmiRelease releases[UMI_DELIVERY_MAX_RELEASES];
    size_t count;
} UmiReleaseRegistry;

void umi_release_registry_init(UmiReleaseRegistry *registry);
UmiStatus umi_release_registry_add(UmiReleaseRegistry *registry,
                                   const UmiRelease *release);
const UmiRelease *umi_release_registry_find(const UmiReleaseRegistry *registry,
                                            const char *release_id);
const UmiRelease *umi_release_registry_latest(const UmiReleaseRegistry *registry,
                                              UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif

#endif
