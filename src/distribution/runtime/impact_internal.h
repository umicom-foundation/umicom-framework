/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/impact_internal.h
 * Purpose: Share bounded path and metadata parsing within the impact planner.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RELEASE_IMPACT_INTERNAL_H
#define UMICOM_RELEASE_IMPACT_INTERNAL_H
#include "umicom/distribution/runtime/impact.h"

typedef UmiStatus (*UmiImpactFieldFn)(void *context, const char *key, const char *value);
UmiStatus UmiImpactMetadataRead(const char *detail, UmiImpactFieldFn field,
    void *context, bool *present);
UmiStatus UmiImpactRelativePath(const char *root, const char *input, char **outPath);
UmiStatus UmiImpactChangedPath(const char *input, char **outPath);
bool UmiImpactBuildConfiguration(const char *path);
struct UmiReleaseImpactPlan {
    UmiReleaseImpactSummary summary;
    UmiReleaseImpactItem *items;
    size_t count;
    /* Owned identity prevents a later run from pairing this plan with another inventory. */
    char inventoryFingerprint[65];
};
#endif
