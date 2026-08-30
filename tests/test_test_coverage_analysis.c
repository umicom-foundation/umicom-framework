/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_coverage_analysis.c
 *
 * PURPOSE:
 *   Implement the test test coverage analysis behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework professional coverage analysis tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/coverage_analysis.h"

static UmiTestPlatformCoverageSnapshot coverage(const char *id, const char *uri,
    uint64_t total, uint64_t covered, uint64_t revision)
{
    UmiTestPlatformCoverageSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_COVERAGE_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.session_id, "run", sizeof(value.session_id) - 1U);
    (void)strncpy(value.uri, uri, sizeof(value.uri) - 1U);
    value.lines_total = total;
    value.lines_covered = covered;
    value.branches_total = 10U;
    value.branches_covered = covered / 10U;
    value.revision = revision;
    return value;
}

int main(void)
{
    UmiTestPlatformCoverageRegistry *registry = NULL;
    UmiTestCoverageAnalysis *analysis = NULL;
    UmiTestCoveragePolicy policy;
    UmiTestCoverageBaseline baseline;
    UmiTestCoverageAnalysisSnapshot snapshot;
    UmiTestCoverageFileAnalysis file;
    UmiTestPlatformCoverageSnapshot first = coverage("a", "a.c", 100U, 90U, 1U);
    UmiTestPlatformCoverageSnapshot second = coverage("b", "b.c", 100U, 50U, 2U);
    assert(umi_test_platform_coverage_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_test_platform_coverage_registry_upsert(registry, &first) == UMI_STATUS_OK);
    assert(umi_test_platform_coverage_registry_upsert(registry, &second) == UMI_STATUS_OK);
    umi_test_coverage_policy_init(&policy);
    umi_test_coverage_baseline_init(&baseline);
    baseline.line_basis_points = 6000U;
    policy.minimum_line_basis_points = 7500U;
    assert(umi_test_coverage_analysis_create(&analysis) == UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_build(analysis, registry, "run", &policy,
                                             &baseline) == UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_snapshot(analysis, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.file_count == 2U && snapshot.failed_file_count == 1U);
    assert(snapshot.line_basis_points == 7000U);
    assert(umi_test_coverage_analysis_find(analysis, "a.c", &file) == UMI_STATUS_OK);
    assert(file.gate_state == UMI_TEST_COVERAGE_GATE_PASSED);
    umi_test_coverage_analysis_destroy(analysis);
    umi_test_platform_coverage_registry_destroy(registry);
    return 0;
}
