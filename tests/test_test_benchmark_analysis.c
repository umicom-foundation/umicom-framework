/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_benchmark_analysis.c
 *
 * PURPOSE:
 *   Verify scaled benchmark statistics, regression gates and atomic failure handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/benchmark_analysis.h"

/*
 * Exercise sample and return a clear result when the behaviour no longer matches its
 * contract.
 */
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Retain the original regression-gate smoke fixture for review; the active
 * dispatcher also covers scale, invalid input and transactional ownership. */
#if 0
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
#endif
static int Smoke(void)
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

#include <float.h>
#include <math.h>

static void KeepBenchmark(UmiTestBenchmarkAnalysis *analysis,
    const UmiTestBenchmarkAnalysisSnapshot *before, const UmiTestBenchmarkMetricAnalysis *metric)
{
    UmiTestBenchmarkAnalysisSnapshot after;
    UmiTestBenchmarkMetricAnalysis retained;
    assert(umi_test_benchmark_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
    assert(memcmp(before,&after,sizeof(after))==0);
    assert(umi_test_benchmark_analysis_find(analysis,"latency","ms",&retained)==UMI_STATUS_OK);
    assert(memcmp(metric,&retained,sizeof(retained))==0);
    assert(umi_test_benchmark_analysis_count(analysis)==1U);
}
static void Near(double actual,double expected)
{
    assert(isfinite(actual));
    if(expected==0.0)assert(actual==0.0);
    else assert(fabs((actual-expected)/expected)<1e-12);
}
int main(int argc,char **argv)
{
    if(argc==1)return Smoke();
    if(argc!=2)return 2;
    const char *name=argv[1];
    UmiTestPlatformBenchmarkRegistry *registry=NULL;
    UmiTestBenchmarkAnalysis *analysis=NULL;
    assert(umi_test_platform_benchmark_registry_create(&registry)==UMI_STATUS_OK);
    assert(umi_test_benchmark_analysis_create(&analysis)==UMI_STATUS_OK);
    UmiTestPlatformBenchmarkSnapshot samples[3]={sample("a",10.0),sample("b",10.0),sample("c",10.0)};
    for(size_t i=0U;i<3U;++i)
        assert(umi_test_platform_benchmark_registry_upsert(registry,&samples[i])==UMI_STATUS_OK);
    UmiTestBenchmarkPolicy policy;umi_test_benchmark_policy_init(&policy);
    policy.minimum_sample_count=3U;
    assert(umi_test_benchmark_analysis_build(analysis,registry,&policy)==UMI_STATUS_OK);
    UmiTestBenchmarkAnalysisSnapshot before,after;
    UmiTestBenchmarkMetricAnalysis prior,metric;
    assert(umi_test_benchmark_analysis_snapshot(analysis,&before)==UMI_STATUS_OK);
    assert(umi_test_benchmark_analysis_find(analysis,"latency","ms",&prior)==UMI_STATUS_OK);
    UmiStatus expected=UMI_STATUS_OK;
    UmiTestBenchmarkGateState gate=UMI_TEST_BENCHMARK_GATE_PASSED;
    size_t count=3U;int known=1;
    if(strcmp(name,"nan-value")==0 || strcmp(name,"infinite-value")==0 ||
       strcmp(name,"nan-baseline")==0 || strcmp(name,"infinite-baseline")==0) {
        double invalid=strstr(name,"nan")!=NULL?NAN:INFINITY;
        if(strstr(name,"baseline")!=NULL)samples[2].baseline=invalid;else samples[2].value=invalid;
        expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(name,"policy-nan-regression")==0 || strcmp(name,"policy-infinite-improvement")==0) {
        if(strcmp(name,"policy-nan-regression")==0)policy.regression_tolerance_percent=NAN;
        else policy.improvement_threshold_percent=INFINITY;
        expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(name,"maximum-constant")==0) {
        for(size_t i=0U;i<3U;++i)samples[i].value=samples[i].baseline=DBL_MAX;
    } else if(strcmp(name,"small-scale")==0 || strcmp(name,"large-scale")==0) {
        double scale=strcmp(name,"small-scale")==0?1e-150:1e150;
        samples[0].value=scale;samples[1].value=3.0*scale;
        samples[0].baseline=samples[1].baseline=2.0*scale;
        count=2U;policy.minimum_sample_count=2U;
    } else if(strcmp(name,"extreme-signed")==0) {
        samples[0].value=-DBL_MAX;samples[1].value=DBL_MAX;
        samples[0].baseline=samples[1].baseline=0.0;
        count=2U;policy.minimum_sample_count=2U;gate=UMI_TEST_BENCHMARK_GATE_UNKNOWN;
    } else if(strcmp(name,"cancelled-baseline")==0) {
        samples[0].baseline=-10.0;samples[1].baseline=10.0;samples[2].baseline=0.0;
        gate=UMI_TEST_BENCHMARK_GATE_UNKNOWN;
    } else if(strcmp(name,"opposite-baseline")==0) {
        for(size_t i=0U;i<3U;++i){samples[i].value=DBL_MAX;samples[i].baseline=-DBL_MAX;}
        gate=UMI_TEST_BENCHMARK_GATE_REGRESSED;
    } else if(strcmp(name,"delta-overflow")==0) {
        for(size_t i=0U;i<3U;++i){samples[i].value=DBL_MAX;samples[i].baseline=1.0;}
        expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if(strcmp(name,"baseline-absent")==0 || strcmp(name,"provider-regression")==0 ||
              strcmp(name,"provider-disabled")==0) {
        for(size_t i=0U;i<3U;++i)samples[i].baseline=0.0;
        gate=UMI_TEST_BENCHMARK_GATE_UNKNOWN;
        if(strcmp(name,"baseline-absent")!=0)samples[0].regression=1;
        if(strcmp(name,"provider-disabled")==0)policy.honour_provider_regression=0;
        if(strcmp(name,"provider-regression")==0){policy.honour_provider_regression=1;gate=UMI_TEST_BENCHMARK_GATE_REGRESSED;}
    } else if(strcmp(name,"negative-values")==0) {
        samples[0].value=-3.0;samples[1].value=-2.0;samples[2].value=-1.0;
        for(size_t i=0U;i<3U;++i)samples[i].baseline=-2.0;
    } else if(strcmp(name,"higher-direction")==0) {
        for(size_t i=0U;i<3U;++i)samples[i].value=12.0;
        policy.direction=UMI_TEST_BENCHMARK_HIGHER_IS_BETTER;gate=UMI_TEST_BENCHMARK_GATE_IMPROVED;
    } else if(strcmp(name,"short-samples")==0) {
        count=1U;gate=UMI_TEST_BENCHMARK_GATE_UNKNOWN;
    } else if(strcmp(name,"late-invalid-metric")==0) {
        UmiTestPlatformBenchmarkSnapshot extra=sample("bad",NAN);strcpy(extra.metric,"other");
        assert(umi_test_platform_benchmark_registry_upsert(registry,&extra)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(name,"arguments")==0) {
        assert(umi_test_benchmark_analysis_build(NULL,registry,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_benchmark_analysis_build(analysis,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        KeepBenchmark(analysis,&before,&prior);
    } else known=0;
    if(known && strcmp(name,"arguments")!=0) {
        for(size_t i=count;i<3U;++i)
            assert(umi_test_platform_benchmark_registry_remove(registry,samples[i].id)==UMI_STATUS_OK);
        for(size_t i=0U;i<count;++i)
            assert(umi_test_platform_benchmark_registry_upsert(registry,&samples[i])==UMI_STATUS_OK);
        UmiStatus status=umi_test_benchmark_analysis_build(analysis,registry,&policy);
        assert(status==expected);
        if(status!=UMI_STATUS_OK)KeepBenchmark(analysis,&before,&prior);
        else {
            assert(umi_test_benchmark_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
            assert(umi_test_benchmark_analysis_find(analysis,"latency","ms",&metric)==UMI_STATUS_OK);
            assert(metric.sample_count==count && metric.gate_state==gate);
            assert(after.revision==before.revision+1U);
            assert(isfinite(metric.mean) && isfinite(metric.standard_deviation) && isfinite(metric.delta_percent));
            if(strcmp(name,"maximum-constant")==0){Near(metric.mean,DBL_MAX);Near(metric.standard_deviation,0.0);Near(metric.delta_percent,0.0);}
            if(strcmp(name,"small-scale")==0 || strcmp(name,"large-scale")==0) {
                double scale=strcmp(name,"small-scale")==0?1e-150:1e150;
                Near(metric.mean,2.0*scale);Near(metric.median,2.0*scale);
                Near(metric.standard_deviation,scale);Near(metric.percentile_90,2.8*scale);
            }
            if(strcmp(name,"extreme-signed")==0){Near(metric.mean,0.0);Near(metric.median,0.0);Near(metric.standard_deviation,DBL_MAX);}
            if(strcmp(name,"opposite-baseline")==0)Near(metric.delta_percent,200.0);
            if(strcmp(name,"negative-values")==0){Near(metric.mean,-2.0);Near(metric.standard_deviation,0.816496580927726);}
            if(strcmp(name,"cancelled-baseline")==0)Near(metric.baseline,0.0);
        }
    }
    umi_test_benchmark_analysis_destroy(analysis);
    umi_test_platform_benchmark_registry_destroy(registry);
    return known?0:2;
}
