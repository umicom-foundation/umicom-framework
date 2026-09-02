/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/types.h
 *
 * PURPOSE:
 *   Define stable delivery identifiers, capacities, stages, package formats and shared records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Every packaging and release module uses these common types so products do not invent incompatible delivery vocabulary.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_TYPES_H
#define INCLUDE_UMICOM_DELIVERY_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DELIVERY_ID_CAPACITY 128U
#define UMI_DELIVERY_PATH_CAPACITY 512U
#define UMI_DELIVERY_TEXT_CAPACITY 1024U
#define UMI_DELIVERY_DIGEST_CAPACITY 96U
#define UMI_DELIVERY_VERSION_CAPACITY 64U
#define UMI_DELIVERY_MAX_ARTIFACTS 64U
#define UMI_DELIVERY_MAX_COMPONENTS 128U
#define UMI_DELIVERY_MAX_LICENSES 64U
#define UMI_DELIVERY_MAX_RELEASES 32U
#define UMI_DELIVERY_MAX_CHECKS 64U
#define UMI_DELIVERY_MAX_OPERATIONS 128U
#define UMI_DELIVERY_MAX_METADATA 64U

/**
 * List the named delivery stage values accepted by this public contract.
 */
typedef enum UmiDeliveryStage {
    UMI_DELIVERY_STAGE_SOURCE = 1,
    UMI_DELIVERY_STAGE_BUILD = 2,
    UMI_DELIVERY_STAGE_TEST = 3,
    UMI_DELIVERY_STAGE_STAGE = 4,
    UMI_DELIVERY_STAGE_PACKAGE = 5,
    UMI_DELIVERY_STAGE_VERIFY = 6,
    UMI_DELIVERY_STAGE_PUBLISH = 7,
    UMI_DELIVERY_STAGE_DEPLOY = 8,
    UMI_DELIVERY_STAGE_HEALTH = 9,
    UMI_DELIVERY_STAGE_COMPLETE = 10
} UmiDeliveryStage;

/**
 * List the named artifact kind values accepted by this public contract.
 */
typedef enum UmiArtifactKind {
    UMI_ARTIFACT_EXECUTABLE = 1,
    UMI_ARTIFACT_LIBRARY = 2,
    UMI_ARTIFACT_ARCHIVE = 3,
    UMI_ARTIFACT_INSTALLER = 4,
    UMI_ARTIFACT_MANIFEST = 5,
    UMI_ARTIFACT_CHECKSUM = 6,
    UMI_ARTIFACT_SBOM = 7,
    UMI_ARTIFACT_LICENSE = 8,
    UMI_ARTIFACT_PROVENANCE = 9
} UmiArtifactKind;

/**
 * List the named package format values accepted by this public contract.
 */
typedef enum UmiPackageFormat {
    UMI_PACKAGE_DIRECTORY = 1,
    UMI_PACKAGE_ZIP = 2,
    UMI_PACKAGE_TAR_GZ = 3,
    UMI_PACKAGE_WINDOWS_SETUP = 4,
    UMI_PACKAGE_MSIX = 5,
    UMI_PACKAGE_FLATPAK = 6
} UmiPackageFormat;

/**
 * List the named release channel values accepted by this public contract.
 */
typedef enum UmiReleaseChannel {
    UMI_RELEASE_DEVELOPMENT = 1,
    UMI_RELEASE_NIGHTLY = 2,
    UMI_RELEASE_BETA = 3,
    UMI_RELEASE_STABLE = 4
} UmiReleaseChannel;

/**
 * List the named evidence status values accepted by this public contract.
 */
typedef enum UmiEvidenceStatus {
    UMI_EVIDENCE_UNKNOWN = 0,
    UMI_EVIDENCE_PASS = 1,
    UMI_EVIDENCE_FAIL = 2,
    UMI_EVIDENCE_SKIP = 3
} UmiEvidenceStatus;

/**
 * List the named update decision values accepted by this public contract.
 */
typedef enum UmiUpdateDecision {
    UMI_UPDATE_NONE = 0,
    UMI_UPDATE_AVAILABLE = 1,
    UMI_UPDATE_REQUIRED = 2,
    UMI_UPDATE_BLOCKED = 3
} UmiUpdateDecision;

/**
 * Provide the delivery stage text operation used by this module and its client
 * applications.
 */
const char *umi_delivery_stage_text(UmiDeliveryStage stage);
/**
 * Provide the artifact kind text operation used by this module and its client
 * applications.
 */
const char *umi_artifact_kind_text(UmiArtifactKind kind);
/**
 * Provide the package format text operation used by this module and its client
 * applications.
 */
const char *umi_package_format_text(UmiPackageFormat format);
/**
 * Provide the release channel text operation used by this module and its client
 * applications.
 */
const char *umi_release_channel_text(UmiReleaseChannel channel);
/**
 * Provide the evidence status text operation used by this module and its client
 * applications.
 */
const char *umi_evidence_status_text(UmiEvidenceStatus status);
/**
 * Provide the update decision text operation used by this module and its client
 * applications.
 */
const char *umi_update_decision_text(UmiUpdateDecision decision);

#ifdef __cplusplus
}
#endif

#endif
