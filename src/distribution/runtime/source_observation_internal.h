/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_observation_internal.h
 *
 * PURPOSE:
 *   Keep source path collection private to immutable observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SOURCE_OBSERVATION_INTERNAL_H
#define UMICOM_SOURCE_OBSERVATION_INTERNAL_H
#include "umicom/distribution/runtime/source_observation.h"
struct UmiReleaseSourceObservation {
    UmiReleaseSourceItem *items;
    size_t count;
    char inventoryDigest[65];
    char fingerprint[65];
};
UmiStatus UmiSourceObservationCollect(const UmiReleaseInventory *inventory,
    UmiReleaseSourceObservation *observation, const char **problemIdentity);
/* Recompute the live format identity when reopening historical values. */
UmiStatus UmiSourceObservationFingerprint(UmiReleaseSourceObservation *observation);
#endif
