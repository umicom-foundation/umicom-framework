/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/package_query.h
 * PURPOSE: Filter and rank the unified package catalogue without shared sort state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_PACKAGE_QUERY_H
#define UMICOM_DEVELOPER_ECOSYSTEM_PACKAGE_QUERY_H

#include "umicom/developer/ecosystem/package_catalogue.h"
#include "umicom/developer/ecosystem/source_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ecosystem package query data shared with callers of this public contract.
 */
typedef struct UmiEcosystemPackageQuery {
    char text[UMI_ECOSYSTEM_NAME_CAPACITY];
    char source_id[UMI_ECOSYSTEM_ID_CAPACITY];
    int package_kind;
    bool installed_only;
    bool updates_only;
    bool verified_only;
    bool compatible_only;
    bool available_sources_only;
    size_t result_limit;
} UmiEcosystemPackageQuery;

/**
 * Represent the ecosystem package query result data shared with callers of this public
 * contract.
 */
typedef struct UmiEcosystemPackageQueryResult {
    size_t package_indexes[UMI_ECOSYSTEM_MAX_RESULTS];
    uint32_t scores[UMI_ECOSYSTEM_MAX_RESULTS];
    size_t result_count;
    size_t total_matches;
    uint64_t catalogue_revision;
    uint64_t source_revision;
} UmiEcosystemPackageQueryResult;

/**
 * Initialise ecosystem package query from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_package_query_init(UmiEcosystemPackageQuery *query);
/**
 * Perform ecosystem package query through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_ecosystem_package_query_execute(
    const UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemSourceCatalogue *sources,
    const UmiEcosystemPackageQuery *query,
    UmiEcosystemPackageQueryResult *out_result);
/**
 * Find ecosystem package query result while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEcosystemPackageRecord *umi_ecosystem_package_query_result_at(
    const UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemPackageQueryResult *result,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_PACKAGE_QUERY_H */
