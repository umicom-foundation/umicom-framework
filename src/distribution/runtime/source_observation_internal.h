/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_observation_internal.h
 * Purpose: Keep source path collection private to immutable observations.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
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
#endif
