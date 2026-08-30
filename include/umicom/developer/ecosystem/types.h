/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/types.h
 *
 * PURPOSE:
 *   Define bounded, toolkit-neutral types for package, SDK, extension and
 *   component-ecosystem workbenches. Existing Framework engines remain the
 *   authorities for installation, plug-in hosting, signing and distribution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_TYPES_H
#define UMICOM_DEVELOPER_ECOSYSTEM_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ECOSYSTEM_API_VERSION 1U
#define UMI_ECOSYSTEM_ID_CAPACITY 96U
#define UMI_ECOSYSTEM_NAME_CAPACITY 160U
#define UMI_ECOSYSTEM_TEXT_CAPACITY 512U
#define UMI_ECOSYSTEM_PATH_CAPACITY 512U
#define UMI_ECOSYSTEM_VERSION_CAPACITY 64U
#define UMI_ECOSYSTEM_MAX_SOURCES 32U
#define UMI_ECOSYSTEM_MAX_PACKAGES 256U
#define UMI_ECOSYSTEM_MAX_RESULTS 128U
#define UMI_ECOSYSTEM_MAX_ACTIONS 64U
#define UMI_ECOSYSTEM_MAX_UPDATES 128U
#define UMI_ECOSYSTEM_MAX_SDKS 64U
#define UMI_ECOSYSTEM_MAX_TEMPLATES 128U
#define UMI_ECOSYSTEM_MAX_GENERATED_FILES 64U
#define UMI_ECOSYSTEM_MAX_MIGRATION_STEPS 64U
#define UMI_ECOSYSTEM_MAX_COMMANDS 40U

typedef enum UmiEcosystemSourceKind {
    UMI_ECOSYSTEM_SOURCE_REMOTE = 0,
    UMI_ECOSYSTEM_SOURCE_LOCAL = 1,
    UMI_ECOSYSTEM_SOURCE_OFFLINE = 2
} UmiEcosystemSourceKind;

typedef enum UmiEcosystemPackageKind {
    UMI_ECOSYSTEM_PACKAGE_EXTENSION = 0,
    UMI_ECOSYSTEM_PACKAGE_SDK = 1,
    UMI_ECOSYSTEM_PACKAGE_COMPONENT = 2,
    UMI_ECOSYSTEM_PACKAGE_TEMPLATE = 3,
    UMI_ECOSYSTEM_PACKAGE_TOOLCHAIN = 4
} UmiEcosystemPackageKind;

typedef enum UmiEcosystemInstallState {
    UMI_ECOSYSTEM_AVAILABLE = 0,
    UMI_ECOSYSTEM_INSTALLED = 1,
    UMI_ECOSYSTEM_DISABLED = 2,
    UMI_ECOSYSTEM_ACTIVE = 3,
    UMI_ECOSYSTEM_UPDATE_AVAILABLE = 4
} UmiEcosystemInstallState;

typedef enum UmiEcosystemCompatibility {
    UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN = 0,
    UMI_ECOSYSTEM_COMPATIBLE = 1,
    UMI_ECOSYSTEM_INCOMPATIBLE = 2,
    UMI_ECOSYSTEM_REQUIRES_MIGRATION = 3
} UmiEcosystemCompatibility;

typedef enum UmiEcosystemEvidenceState {
    UMI_ECOSYSTEM_EVIDENCE_UNKNOWN = 0,
    UMI_ECOSYSTEM_EVIDENCE_VERIFIED = 1,
    UMI_ECOSYSTEM_EVIDENCE_WARNING = 2,
    UMI_ECOSYSTEM_EVIDENCE_REJECTED = 3
} UmiEcosystemEvidenceState;

typedef enum UmiEcosystemReviewSeverity {
    UMI_ECOSYSTEM_REVIEW_INFORMATION = 0,
    UMI_ECOSYSTEM_REVIEW_WARNING = 1,
    UMI_ECOSYSTEM_REVIEW_BLOCKER = 2
} UmiEcosystemReviewSeverity;

typedef enum UmiEcosystemActionKind {
    UMI_ECOSYSTEM_ACTION_DOWNLOAD = 0,
    UMI_ECOSYSTEM_ACTION_VERIFY = 1,
    UMI_ECOSYSTEM_ACTION_RESOLVE_DEPENDENCIES = 2,
    UMI_ECOSYSTEM_ACTION_REVIEW_PERMISSIONS = 3,
    UMI_ECOSYSTEM_ACTION_INSTALL = 4,
    UMI_ECOSYSTEM_ACTION_UPDATE = 5,
    UMI_ECOSYSTEM_ACTION_ACTIVATE = 6,
    UMI_ECOSYSTEM_ACTION_MIGRATE = 7,
    UMI_ECOSYSTEM_ACTION_RESTART = 8,
    UMI_ECOSYSTEM_ACTION_ROLLBACK = 9
} UmiEcosystemActionKind;

typedef enum UmiEcosystemMigrationKind {
    UMI_ECOSYSTEM_MIGRATION_MANIFEST = 0,
    UMI_ECOSYSTEM_MIGRATION_PROJECT = 1,
    UMI_ECOSYSTEM_MIGRATION_SETTINGS = 2,
    UMI_ECOSYSTEM_MIGRATION_COMPONENT = 3,
    UMI_ECOSYSTEM_MIGRATION_CACHE = 4
} UmiEcosystemMigrationKind;

UmiStatus umi_ecosystem_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
bool umi_ecosystem_text_contains(const char *text, const char *query);
const char *umi_ecosystem_source_kind_text(UmiEcosystemSourceKind kind);
const char *umi_ecosystem_package_kind_text(UmiEcosystemPackageKind kind);
const char *umi_ecosystem_install_state_text(UmiEcosystemInstallState state);
const char *umi_ecosystem_compatibility_text(UmiEcosystemCompatibility value);
const char *umi_ecosystem_evidence_state_text(UmiEcosystemEvidenceState state);
const char *umi_ecosystem_action_kind_text(UmiEcosystemActionKind kind);
const char *umi_ecosystem_migration_kind_text(UmiEcosystemMigrationKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_TYPES_H */
