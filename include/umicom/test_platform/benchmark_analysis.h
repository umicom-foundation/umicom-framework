/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/benchmark_analysis.h
 *
 * PURPOSE:
 *   Calculate stable benchmark distributions, percentiles and regression gates
 *   from the canonical benchmark registry without introducing a statistics
 *   dependency or application-specific analysis code.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BENCHMARK_ANALYSIS_H
#define UMICOM_TEST_PLATFORM_BENCHMARK_ANALYSIS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/test_platform/benchmark.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_BENCHMARK_ANALYSIS_API_VERSION 1U

typedef enum UmiTestBenchmarkDirection {
    UMI_TEST_BENCHMARK_LOWER_IS_BETTER = 1,
    UMI_TEST_BENCHMARK_HIGHER_IS_BETTER = 2
} UmiTestBenchmarkDirection;

typedef enum UmiTestBenchmarkGateState {
    UMI_TEST_BENCHMARK_GATE_UNKNOWN = 0,
    UMI_TEST_BENCHMARK_GATE_PASSED = 1,
    UMI_TEST_BENCHMARK_GATE_IMPROVED = 2,
    UMI_TEST_BENCHMARK_GATE_REGRESSED = 3
} UmiTestBenchmarkGateState;

typedef struct UmiTestBenchmarkPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    size_t minimum_sample_count;
    double regression_tolerance_percent;
    double improvement_threshold_percent;
    UmiTestBenchmarkDirection direction;
    int honour_provider_regression;
} UmiTestBenchmarkPolicy;

typedef struct UmiTestBenchmarkMetricAnalysis {
    uint32_t struct_size;
    uint32_t api_version;
    char metric[128];
    char unit[64];
    size_t sample_count;
    double minimum;
    double maximum;
    double mean;
    double median;
    double percentile_90;
    double percentile_95;
    double percentile_99;
    double standard_deviation;
    double baseline;
    double delta_percent;
    UmiTestBenchmarkGateState gate_state;
    size_t provider_regression_count;
} UmiTestBenchmarkMetricAnalysis;

typedef struct UmiTestBenchmarkAnalysisSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t metric_count;
    size_t sample_count;
    size_t passed_metric_count;
    size_t improved_metric_count;
    size_t regressed_metric_count;
    uint64_t source_revision;
    uint64_t revision;
} UmiTestBenchmarkAnalysisSnapshot;

typedef struct UmiTestBenchmarkAnalysis UmiTestBenchmarkAnalysis;

void umi_test_benchmark_policy_init(UmiTestBenchmarkPolicy *policy);
UmiStatus umi_test_benchmark_analysis_create(
    UmiTestBenchmarkAnalysis **out_analysis);
void umi_test_benchmark_analysis_destroy(UmiTestBenchmarkAnalysis *analysis);
UmiStatus umi_test_benchmark_analysis_build(
    UmiTestBenchmarkAnalysis *analysis,
    const UmiTestPlatformBenchmarkRegistry *benchmarks,
    const UmiTestBenchmarkPolicy *policy);
UmiStatus umi_test_benchmark_analysis_at(
    const UmiTestBenchmarkAnalysis *analysis,
    size_t position,
    UmiTestBenchmarkMetricAnalysis *out_metric);
UmiStatus umi_test_benchmark_analysis_find(
    const UmiTestBenchmarkAnalysis *analysis,
    const char *metric,
    const char *unit,
    UmiTestBenchmarkMetricAnalysis *out_metric);
UmiStatus umi_test_benchmark_analysis_snapshot(
    const UmiTestBenchmarkAnalysis *analysis,
    UmiTestBenchmarkAnalysisSnapshot *out_snapshot);
size_t umi_test_benchmark_analysis_count(
    const UmiTestBenchmarkAnalysis *analysis);
const char *umi_test_benchmark_gate_state_text(UmiTestBenchmarkGateState state);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_BENCHMARK_ANALYSIS_H */
