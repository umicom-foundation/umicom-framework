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

typedef struct UmiEcosystemPackageQueryResult {
    size_t package_indexes[UMI_ECOSYSTEM_MAX_RESULTS];
    uint32_t scores[UMI_ECOSYSTEM_MAX_RESULTS];
    size_t result_count;
    size_t total_matches;
    uint64_t catalogue_revision;
    uint64_t source_revision;
} UmiEcosystemPackageQueryResult;

void umi_ecosystem_package_query_init(UmiEcosystemPackageQuery *query);
UmiStatus umi_ecosystem_package_query_execute(
    const UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemSourceCatalogue *sources,
    const UmiEcosystemPackageQuery *query,
    UmiEcosystemPackageQueryResult *out_result);
const UmiEcosystemPackageRecord *umi_ecosystem_package_query_result_at(
    const UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemPackageQueryResult *result,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_PACKAGE_QUERY_H */
