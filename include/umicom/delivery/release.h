/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release.h
 *
 * PURPOSE:
 *   Represent one immutable release candidate and its promotion state.
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

#ifndef INCLUDE_UMICOM_DELIVERY_RELEASE_H
#define INCLUDE_UMICOM_DELIVERY_RELEASE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the release data shared with callers of this public contract.
 */
typedef struct UmiRelease {
    UmiDeliveryManifest manifest;
    uint64_t generation;
    UmiEvidenceStatus verification;
    int published;
    int deployed;
    int healthy;
} UmiRelease;

/**
 * Initialise release from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_release_init(UmiRelease *release,
                           const UmiDeliveryManifest *manifest,
                           uint64_t generation);
/**
 * Provide the release ready to publish operation used by this module and its client
 * applications.
 */
int umi_release_ready_to_publish(const UmiRelease *release);
/**
 * Provide the release mark published operation used by this module and its client
 * applications.
 */
void umi_release_mark_published(UmiRelease *release);

#ifdef __cplusplus
}
#endif

#endif
