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

/**
 * Represent the release registry data shared with callers of this public contract.
 */
typedef struct UmiReleaseRegistry {
    UmiRelease releases[UMI_DELIVERY_MAX_RELEASES];
    size_t count;
} UmiReleaseRegistry;

/**
 * Initialise release registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_release_registry_init(UmiReleaseRegistry *registry);
/**
 * Add release registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_release_registry_add(UmiReleaseRegistry *registry,
                                   const UmiRelease *release);
/**
 * Find release registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiRelease *umi_release_registry_find(const UmiReleaseRegistry *registry,
                                            const char *release_id);
/**
 * Provide the release registry latest operation used by this module and its client
 * applications.
 */
const UmiRelease *umi_release_registry_latest(const UmiReleaseRegistry *registry,
                                              UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif

#endif
