/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/runtime.h
 * PURPOSE: Coordinate package, source, SDK, component and migration workbenches.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_RUNTIME_H
#define UMICOM_DEVELOPER_ECOSYSTEM_RUNTIME_H

#include "umicom/developer/ecosystem/component_wizard.h"
#include "umicom/developer/ecosystem/install_review.h"
#include "umicom/developer/ecosystem/migration_review.h"
#include "umicom/developer/ecosystem/package_query.h"
#include "umicom/developer/ecosystem/provenance_review.h"
#include "umicom/developer/ecosystem/sdk_inventory.h"
#include "umicom/developer/ecosystem/update_review.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ecosystem runtime snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEcosystemRuntimeSnapshot {
    size_t source_count;
    size_t available_source_count;
    size_t package_count;
    size_t installed_package_count;
    size_t update_count;
    size_t sdk_count;
    size_t installed_sdk_count;
    size_t template_count;
    bool offline_mode;
    uint64_t revision;
} UmiEcosystemRuntimeSnapshot;

/**
 * Represent the ecosystem runtime data shared with callers of this public contract.
 */
typedef struct UmiEcosystemRuntime UmiEcosystemRuntime;

/**
 * Initialise ecosystem runtime from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ecosystem_runtime_create(UmiEcosystemRuntime **out_runtime);
/**
 * Release or reset state held by ecosystem runtime so the same storage can be reused
 * safely.
 */
void umi_ecosystem_runtime_destroy(UmiEcosystemRuntime *runtime);
/**
 * Provide the ecosystem runtime sources operation used by this module and its client
 * applications.
 */
UmiEcosystemSourceCatalogue *umi_ecosystem_runtime_sources(
    UmiEcosystemRuntime *runtime);
/**
 * Provide the ecosystem runtime packages operation used by this module and its client
 * applications.
 */
UmiEcosystemPackageCatalogue *umi_ecosystem_runtime_packages(
    UmiEcosystemRuntime *runtime);
/**
 * Provide the ecosystem runtime sdks operation used by this module and its client
 * applications.
 */
UmiEcosystemSdkInventory *umi_ecosystem_runtime_sdks(
    UmiEcosystemRuntime *runtime);
/**
 * Provide the ecosystem runtime components operation used by this module and its client
 * applications.
 */
UmiEcosystemComponentWizard *umi_ecosystem_runtime_components(
    UmiEcosystemRuntime *runtime);
/**
 * Provide the ecosystem runtime set offline operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_runtime_set_offline(
    UmiEcosystemRuntime *runtime,
    bool offline_mode);
/**
 * Provide the ecosystem runtime query operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_runtime_query(
    const UmiEcosystemRuntime *runtime,
    const UmiEcosystemPackageQuery *query,
    UmiEcosystemPackageQueryResult *out_result);
/**
 * Provide the ecosystem runtime prepare install operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_runtime_prepare_install(
    const UmiEcosystemRuntime *runtime,
    const char *package_id,
    UmiEcosystemInstallReview *out_review);
/**
 * Provide the ecosystem runtime prepare updates operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_runtime_prepare_updates(
    const UmiEcosystemRuntime *runtime,
    UmiEcosystemUpdateReview *out_review);
/**
 * Provide the ecosystem runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_runtime_snapshot(
    const UmiEcosystemRuntime *runtime,
    UmiEcosystemRuntimeSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_RUNTIME_H */
