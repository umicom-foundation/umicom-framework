/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/manifest_drift.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_MANIFEST_DRIFT_H
#define UMICOM_APPLICATION_PRODUCTION_MANIFEST_DRIFT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/manifest_snapshot.h"

/**
 * List the named application production manifest drift flags values accepted by this
 * public contract.
 */
typedef enum UmiApplicationProductionManifestDriftFlags {
    UMI_APPLICATION_PRODUCTION_DRIFT_NONE = 0U,
    UMI_APPLICATION_PRODUCTION_DRIFT_IDENTITY = 1U << 0,
    UMI_APPLICATION_PRODUCTION_DRIFT_EXECUTABLE = 1U << 1,
    UMI_APPLICATION_PRODUCTION_DRIFT_FRONTEND = 1U << 2,
    UMI_APPLICATION_PRODUCTION_DRIFT_PANELS = 1U << 3,
    UMI_APPLICATION_PRODUCTION_DRIFT_LAYOUTS = 1U << 4,
    UMI_APPLICATION_PRODUCTION_DRIFT_FEATURES = 1U << 5,
    UMI_APPLICATION_PRODUCTION_DRIFT_AVAILABILITY = 1U << 6
} UmiApplicationProductionManifestDriftFlags;

/**
 * Represent the application production manifest drift data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionManifestDrift {
    uint32_t flags;
    size_t difference_count;
    int compatible;
} UmiApplicationProductionManifestDrift;

/**
 * Provide the application production manifest drift compare operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_manifest_drift_compare(
    const UmiApplicationProductionManifestContract *expected,
    const UmiApplicationProductionManifestSnapshot *actual,
    UmiApplicationProductionManifestDrift *out_drift);

#ifdef __cplusplus
}
#endif
#endif
