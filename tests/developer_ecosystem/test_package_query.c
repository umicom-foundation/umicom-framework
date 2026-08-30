/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_package_query.c
 * PURPOSE: Verify deterministic package filtering, ranking and offline sources.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer/ecosystem/package_query.h"

static void add_package(
    UmiEcosystemPackageCatalogue *catalogue,
    const char *id,
    const char *name,
    const char *source,
    UmiEcosystemPackageKind kind,
    uint32_t rating)
{
    UmiEcosystemPackageRecord record;
    umi_ecosystem_package_record_init(&record, id, name, kind, source, "1.0.0");
    record.compatibility = UMI_ECOSYSTEM_COMPATIBLE;
    record.evidence = UMI_ECOSYSTEM_EVIDENCE_VERIFIED;
    record.rating_milli = rating;
    assert(umi_ecosystem_package_catalogue_upsert(catalogue, &record) ==
        UMI_STATUS_OK);
}

int main(void)
{
    UmiEcosystemPackageCatalogue packages;
    UmiEcosystemSourceCatalogue sources;
    UmiEcosystemSource source;
    UmiEcosystemPackageQuery query;
    UmiEcosystemPackageQueryResult result;
    const UmiEcosystemPackageRecord *first;
    umi_ecosystem_package_catalogue_init(&packages);
    umi_ecosystem_source_catalogue_init(&sources);
    umi_ecosystem_source_init(&source, "local", "Local",
        UMI_ECOSYSTEM_SOURCE_LOCAL, "C:/feed");
    assert(umi_ecosystem_source_catalogue_upsert(&sources, &source) ==
        UMI_STATUS_OK);
    add_package(&packages, "org.umicom.c23", "C23 Tools", "local",
        UMI_ECOSYSTEM_PACKAGE_EXTENSION, 5000U);
    add_package(&packages, "org.umicom.c23.templates", "C23 Templates", "local",
        UMI_ECOSYSTEM_PACKAGE_TEMPLATE, 4000U);
    umi_ecosystem_package_query_init(&query);
    assert(umi_ecosystem_copy_text(query.text, sizeof(query.text), "c23") ==
        UMI_STATUS_OK);
    query.available_sources_only = true;
    assert(umi_ecosystem_package_query_execute(&packages, &sources, &query,
        &result) == UMI_STATUS_OK);
    assert(result.result_count == 2U && result.total_matches == 2U);
    first = umi_ecosystem_package_query_result_at(&packages, &result, 0U);
    assert(first != NULL && strcmp(first->package_id, "org.umicom.c23") == 0);
    return 0;
}
