/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/test_evidence/main.c
 *
 * PURPOSE:
 *   Demonstrate complete JSON export and retained coverage evidence through the SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/structured_report.h"
#include "umicom/test_platform/coverage_analysis.h"
#include "umicom/test_platform/benchmark_analysis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int Expect(UmiStatus actual, UmiStatus expected, const char *step)
{
    if (actual == expected) return 1;
    fprintf(stderr, "%s: expected status %d, received %d\n", step,
        (int)expected, (int)actual);
    return 0;
}
static int ResultExample(int refuse_short)
{
    /* A snapshot owns its fixed-size fields. The formatter only borrows it. */
    UmiTestPlatformResultSnapshot result = {0};
    strcpy(result.id, "result.example");
    strcpy(result.session_id, "run.example");
    strcpy(result.item_id, "example.source-check");
    strcpy(result.message, "Expected \"ready\"\nReceived \"waiting\"");
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    result.duration_ms = 12.5;
    result.exit_code = 1;
    char *json = malloc(UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
    if (json == NULL) { fputs("Cannot allocate the report buffer.\n", stderr); return 1; }
    strcpy(json, "previous complete report");
    UmiStatus status = umi_test_platform_report_result_json(&result, json,
        refuse_short ? 8U : UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
    int ok = Expect(status, refuse_short ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK,
        "export result");
    if (ok && refuse_short) {
        ok = strcmp(json, "previous complete report") == 0;
        puts(ok ? "Short output refused; previous report retained." : "Previous report was changed.");
    } else if (ok) {
        puts(json);
    }
    free(json);
    /* This return code describes the example's checks. The invented result
     * intentionally says failed; printing valid JSON does not change that result. */
    return ok ? 0 : 1;
}
static int CoverageExample(int overflow)
{
    UmiTestPlatformCoverageRegistry *registry = NULL;
    UmiTestCoverageAnalysis *analysis = NULL;
    int ok = Expect(umi_test_platform_coverage_registry_create(&registry),
        UMI_STATUS_OK, "create coverage registry");
    if (ok) ok = Expect(umi_test_coverage_analysis_create(&analysis),
        UMI_STATUS_OK, "create analysis");
    UmiTestPlatformCoverageSnapshot input = {0};
    strcpy(input.id, "fragment.example");
    strcpy(input.session_id, "run.example");
    strcpy(input.uri, "example.c");
    input.lines_total = overflow ? UINT64_MAX : 100U;
    input.lines_covered = overflow ? UINT64_MAX : 90U;
    input.branches_total = 10U;
    input.branches_covered = 8U;
    if (ok) ok = Expect(umi_test_platform_coverage_registry_upsert(registry, &input),
        UMI_STATUS_OK, "store fragment");
    if (ok) ok = Expect(umi_test_coverage_analysis_build(analysis, registry,
        "run.example", NULL, NULL), UMI_STATUS_OK, "analyse coverage");
    UmiTestCoverageAnalysisSnapshot before = {0}, after = {0};
    if (ok) ok = Expect(umi_test_coverage_analysis_snapshot(analysis, &before),
        UMI_STATUS_OK, "read analysis");
    if (ok && overflow) {
        strcpy(input.id, "fragment.extra");
        input.lines_total = input.lines_covered = 1U;
        input.branches_total = input.branches_covered = 0U;
        ok = Expect(umi_test_platform_coverage_registry_upsert(registry, &input),
            UMI_STATUS_OK, "store excessive fragment");
        if (ok) ok = Expect(umi_test_coverage_analysis_build(analysis, registry,
            "run.example", NULL, NULL), UMI_STATUS_CAPACITY_EXCEEDED, "refuse counter overflow");
        if (ok) ok = Expect(umi_test_coverage_analysis_snapshot(analysis, &after),
            UMI_STATUS_OK, "read retained analysis");
        if (ok) ok = before.revision == after.revision &&
            before.lines_total == after.lines_total &&
            before.line_basis_points == after.line_basis_points;
        if (ok) puts("Overflow refused; previous analysis retained.");
    } else if (ok) {
        printf("Line coverage: %u.%02u%% | gate: %s\n",
            (unsigned)(before.line_basis_points / 100U),
            (unsigned)(before.line_basis_points % 100U),
            umi_test_coverage_gate_state_text(before.gate_state));
        ok = before.line_basis_points == 9000U;
    }
    umi_test_coverage_analysis_destroy(analysis);
    umi_test_platform_coverage_registry_destroy(registry);
    return ok ? 0 : 1;
}
static int BenchmarkExample(void)
{
    UmiTestPlatformBenchmarkRegistry *registry = NULL;
    UmiTestBenchmarkAnalysis *analysis = NULL;
    int ok = Expect(umi_test_platform_benchmark_registry_create(&registry),
        UMI_STATUS_OK, "create benchmark registry");
    if (ok) ok = Expect(umi_test_benchmark_analysis_create(&analysis),
        UMI_STATUS_OK, "create benchmark analysis");
    static const char *ids[] = {"sample.first", "sample.second", "sample.third"};
    for (size_t i = 0U; ok && i < 3U; ++i) {
        UmiTestPlatformBenchmarkSnapshot sample = {0};
        strcpy(sample.id, ids[i]); strcpy(sample.metric, "example.latency");
        strcpy(sample.unit, "ms");
        sample.value = 11.0 + (double)i; sample.baseline = 10.0;
        ok = Expect(umi_test_platform_benchmark_registry_upsert(registry, &sample),
            UMI_STATUS_OK, "store benchmark sample");
    }
    UmiTestBenchmarkPolicy policy;
    umi_test_benchmark_policy_init(&policy);
    policy.minimum_sample_count = 3U;
    if (ok) ok = Expect(umi_test_benchmark_analysis_build(analysis, registry, &policy),
        UMI_STATUS_OK, "analyse benchmark samples");
    UmiTestBenchmarkMetricAnalysis metric = {0};
    if (ok) ok = Expect(umi_test_benchmark_analysis_find(analysis, "example.latency", "ms", &metric),
        UMI_STATUS_OK, "read benchmark analysis");
    if (ok) {
        printf("Mean %.3f ms | population deviation %.6f ms | change %.3f%%\n",
            metric.mean, metric.standard_deviation, metric.delta_percent);
        ok = metric.gate_state == UMI_TEST_BENCHMARK_GATE_REGRESSED;
        if (ok) puts("Latency increased beyond the configured tolerance.");
    }
    umi_test_benchmark_analysis_destroy(analysis);
    umi_test_platform_benchmark_registry_destroy(registry);
    return ok ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fputs("Usage: umicom-test-evidence result|short-output|coverage|overflow|benchmark\n", stderr);
        return 2;
    }
    if (strcmp(argv[1], "result") == 0) return ResultExample(0);
    if (strcmp(argv[1], "short-output") == 0) return ResultExample(1);
    if (strcmp(argv[1], "coverage") == 0) return CoverageExample(0);
    if (strcmp(argv[1], "overflow") == 0) return CoverageExample(1);
    if (strcmp(argv[1], "benchmark") == 0) return BenchmarkExample();
    fputs("Unknown example mode.\n", stderr);
    return 2;
}
