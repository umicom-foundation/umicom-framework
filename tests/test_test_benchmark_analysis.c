/*-----------------------------------------------------------------------------
 * Umicom Framework professional benchmark analysis tests.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/benchmark_analysis.h"

static UmiTestPlatformBenchmarkSnapshot sample(const char *id, double value)
{
    UmiTestPlatformBenchmarkSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_TEST_PLATFORM_BENCHMARK_API_VERSION;
    (void)strncpy(item.id, id, sizeof(item.id) - 1U);
    (void)strncpy(item.metric, "latency", sizeof(item.metric) - 1U);
    (void)strncpy(item.unit, "ms", sizeof(item.unit) - 1U);
    item.value = value;
    item.baseline = 10.0;
    return item;
}

int main(void)
{
    UmiTestPlatformBenchmarkRegistry *registry = NULL;
    UmiTestBenchmarkAnalysis *analysis = NULL;
    UmiTestBenchmarkPolicy policy;
    UmiTestBenchmarkMetricAnalysis metric;
    UmiTestBenchmarkAnalysisSnapshot snapshot;
    UmiTestPlatformBenchmarkSnapshot a = sample("a", 11.0);
    UmiTestPlatformBenchmarkSnapshot b = sample("b", 12.0);
    UmiTestPlatformBenchmarkSnapshot c = sample("c", 13.0);
    assert(umi_test_platform_benchmark_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_test_platform_benchmark_registry_upsert(registry, &a) == UMI_STATUS_OK);
    assert(umi_test_platform_benchmark_registry_upsert(registry, &b) == UMI_STATUS_OK);
    assert(umi_test_platform_benchmark_registry_upsert(registry, &c) == UMI_STATUS_OK);
    umi_test_benchmark_policy_init(&policy);
    policy.minimum_sample_count = 3U;
    assert(umi_test_benchmark_analysis_create(&analysis) == UMI_STATUS_OK);
    assert(umi_test_benchmark_analysis_build(analysis, registry, &policy) == UMI_STATUS_OK);
    assert(umi_test_benchmark_analysis_find(analysis, "latency", "ms", &metric) == UMI_STATUS_OK);
    assert(metric.sample_count == 3U && metric.median == 12.0);
    assert(metric.gate_state == UMI_TEST_BENCHMARK_GATE_REGRESSED);
    assert(umi_test_benchmark_analysis_snapshot(analysis, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.regressed_metric_count == 1U);
    umi_test_benchmark_analysis_destroy(analysis);
    umi_test_platform_benchmark_registry_destroy(registry);
    return 0;
}
