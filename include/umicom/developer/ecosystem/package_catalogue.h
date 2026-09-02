/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/package_catalogue.h
 * PURPOSE: Project packages, extensions, SDKs, components and templates into
 *          one bounded catalogue without replacing their owning engines.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_PACKAGE_CATALOGUE_H
#define UMICOM_DEVELOPER_ECOSYSTEM_PACKAGE_CATALOGUE_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ecosystem package record data shared with callers of this public contract.
 */
typedef struct UmiEcosystemPackageRecord {
    char package_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char display_name[UMI_ECOSYSTEM_NAME_CAPACITY];
    char publisher[UMI_ECOSYSTEM_NAME_CAPACITY];
    char description[UMI_ECOSYSTEM_TEXT_CAPACITY];
    char source_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char installed_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char available_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char channel[UMI_ECOSYSTEM_ID_CAPACITY];
    UmiEcosystemPackageKind kind;
    UmiEcosystemInstallState state;
    UmiEcosystemCompatibility compatibility;
    UmiEcosystemEvidenceState evidence;
    bool installed;
    bool enabled;
    bool pinned;
    bool requires_restart;
    bool requires_migration;
    size_t dependency_count;
    size_t permission_count;
    size_t component_count;
    uint64_t download_bytes;
    uint32_t rating_milli;
    uint64_t revision;
} UmiEcosystemPackageRecord;

/**
 * Represent the ecosystem package statistics data shared with callers of this public
 * contract.
 */
typedef struct UmiEcosystemPackageStatistics {
    size_t total;
    size_t installed;
    size_t active;
    size_t updates;
    size_t incompatible;
    size_t unverified;
    size_t extensions;
    size_t sdks;
    size_t components;
    size_t templates;
} UmiEcosystemPackageStatistics;

/**
 * Represent the ecosystem package catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiEcosystemPackageCatalogue {
    UmiEcosystemPackageRecord packages[UMI_ECOSYSTEM_MAX_PACKAGES];
    size_t package_count;
    uint64_t revision;
} UmiEcosystemPackageCatalogue;

/**
 * Initialise ecosystem package record from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_package_record_init(
    UmiEcosystemPackageRecord *record,
    const char *package_id,
    const char *display_name,
    UmiEcosystemPackageKind kind,
    const char *source_id,
    const char *available_version);
/**
 * Check that ecosystem package record satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ecosystem_package_record_validate(
    const UmiEcosystemPackageRecord *record);
/**
 * Provide the ecosystem package has update operation used by this module and its client
 * applications.
 */
bool umi_ecosystem_package_has_update(
    const UmiEcosystemPackageRecord *record);
/**
 * Initialise ecosystem package catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_package_catalogue_init(
    UmiEcosystemPackageCatalogue *catalogue);
/**
 * Provide the ecosystem package catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_package_catalogue_upsert(
    UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemPackageRecord *record);
/**
 * Find ecosystem package catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiEcosystemPackageRecord *umi_ecosystem_package_catalogue_find(
    UmiEcosystemPackageCatalogue *catalogue,
    const char *package_id);
/**
 * Provide the ecosystem package catalogue find const operation used by this module and its
 * client applications.
 */
const UmiEcosystemPackageRecord *umi_ecosystem_package_catalogue_find_const(
    const UmiEcosystemPackageCatalogue *catalogue,
    const char *package_id);
/**
 * Provide the ecosystem package catalogue statistics operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_package_catalogue_statistics(
    const UmiEcosystemPackageCatalogue *catalogue,
    UmiEcosystemPackageStatistics *out_statistics);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_PACKAGE_CATALOGUE_H */
