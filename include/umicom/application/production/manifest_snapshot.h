/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/manifest_snapshot.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_MANIFEST_SNAPSHOT_H
#define UMICOM_APPLICATION_PRODUCTION_MANIFEST_SNAPSHOT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/manifest_contract.h"

/**
 * Represent the application production manifest snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionManifestSnapshot {
    UmiApplicationProductionManifestContract observed;
    int manifest_available;
    int composition_available;
    int executable_available;
    int tests_available;
} UmiApplicationProductionManifestSnapshot;

/**
 * Provide the application production manifest snapshot capture operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_production_manifest_snapshot_capture(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionManifestSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
