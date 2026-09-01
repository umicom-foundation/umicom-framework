/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/types.c
 *
 * PURPOSE:
 *   Translate delivery enumerations into stable human-readable text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Centralising enum text keeps Studio, command-line tools and release logs consistent.
 */

#include "umicom/delivery/types.h"

const char *umi_delivery_stage_text(UmiDeliveryStage stage)
{
    switch (stage) {
        case UMI_DELIVERY_STAGE_SOURCE: return "source";
        case UMI_DELIVERY_STAGE_BUILD: return "build";
        case UMI_DELIVERY_STAGE_TEST: return "test";
        case UMI_DELIVERY_STAGE_STAGE: return "stage";
        case UMI_DELIVERY_STAGE_PACKAGE: return "package";
        case UMI_DELIVERY_STAGE_VERIFY: return "verify";
        case UMI_DELIVERY_STAGE_PUBLISH: return "publish";
        case UMI_DELIVERY_STAGE_DEPLOY: return "deploy";
        case UMI_DELIVERY_STAGE_HEALTH: return "health";
        case UMI_DELIVERY_STAGE_COMPLETE: return "complete";
        default: return "unknown";
    }
}

const char *umi_artifact_kind_text(UmiArtifactKind kind)
{
    switch (kind) {
        case UMI_ARTIFACT_EXECUTABLE: return "executable";
        case UMI_ARTIFACT_LIBRARY: return "library";
        case UMI_ARTIFACT_ARCHIVE: return "archive";
        case UMI_ARTIFACT_INSTALLER: return "installer";
        case UMI_ARTIFACT_MANIFEST: return "manifest";
        case UMI_ARTIFACT_CHECKSUM: return "checksum";
        case UMI_ARTIFACT_SBOM: return "sbom";
        case UMI_ARTIFACT_LICENSE: return "license";
        case UMI_ARTIFACT_PROVENANCE: return "provenance";
        default: return "unknown";
    }
}

const char *umi_package_format_text(UmiPackageFormat format)
{
    switch (format) {
        case UMI_PACKAGE_DIRECTORY: return "directory";
        case UMI_PACKAGE_ZIP: return "zip";
        case UMI_PACKAGE_TAR_GZ: return "tar.gz";
        case UMI_PACKAGE_WINDOWS_SETUP: return "windows-setup";
        case UMI_PACKAGE_MSIX: return "msix";
        case UMI_PACKAGE_FLATPAK: return "flatpak";
        default: return "unknown";
    }
}

const char *umi_release_channel_text(UmiReleaseChannel channel)
{
    switch (channel) {
        case UMI_RELEASE_DEVELOPMENT: return "development";
        case UMI_RELEASE_NIGHTLY: return "nightly";
        case UMI_RELEASE_BETA: return "beta";
        case UMI_RELEASE_STABLE: return "stable";
        default: return "unknown";
    }
}

const char *umi_evidence_status_text(UmiEvidenceStatus status)
{
    switch (status) {
        case UMI_EVIDENCE_UNKNOWN: return "unknown";
        case UMI_EVIDENCE_PASS: return "pass";
        case UMI_EVIDENCE_FAIL: return "fail";
        case UMI_EVIDENCE_SKIP: return "skip";
        default: return "unknown";
    }
}

const char *umi_update_decision_text(UmiUpdateDecision decision)
{
    switch (decision) {
        case UMI_UPDATE_NONE: return "none";
        case UMI_UPDATE_AVAILABLE: return "available";
        case UMI_UPDATE_REQUIRED: return "required";
        case UMI_UPDATE_BLOCKED: return "blocked";
        default: return "unknown";
    }
}
