/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_distribution_repository.c
 *
 * PURPOSE:
 *   Verify repository filtering and deterministic latest-release selection.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>

#include "distribution_fixture.h"

int main(void)
{
    UmiDistributionRepository *repository = NULL;
    UmiDistributionPackage stable = distribution_test_package(
        "studio-0.17.0", "org.umicom.studio", (UmiVersion){0U, 17U, 0U},
        UMI_RELEASE_STABLE);
    UmiDistributionPackage beta = distribution_test_package(
        "studio-0.18.0-beta", "org.umicom.studio", (UmiVersion){0U, 18U, 0U},
        UMI_RELEASE_BETA);
    UmiDistributionPackage result;
    UmiDistributionPackage results[4];
    UmiDistributionQuery query = {0};
    assert(umi_distribution_repository_create(8U, &repository) == UMI_STATUS_OK);
    assert(umi_distribution_repository_upsert(repository, &stable) == UMI_STATUS_OK);
    assert(umi_distribution_repository_upsert(repository, &beta) == UMI_STATUS_OK);
    assert(umi_distribution_repository_latest(
        repository, "org.umicom.studio", UMI_RELEASE_STABLE,
        UMICOM_FRAMEWORK_ABI_VERSION, &result) == UMI_STATUS_OK);
    assert(result.version.minor == 17U);
    assert(umi_distribution_repository_latest(
        repository, "org.umicom.studio", UMI_RELEASE_BETA,
        UMICOM_FRAMEWORK_ABI_VERSION, &result) == UMI_STATUS_OK);
    assert(result.version.minor == 18U);
    query.text = "Studio";
    query.trusted_only = 1;
    query.compatible_only = 1;
    query.include_deprecated = 0;
    assert(umi_distribution_repository_query(
        repository, &query, results, 4U) == 2U);
    umi_distribution_repository_destroy(repository);
    return 0;
}
