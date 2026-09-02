/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/benchmark_analysis.h
 *
 * PURPOSE:
 *   Calculate stable benchmark distributions, percentiles and regression gates
 *   from the canonical benchmark registry without introducing a statistics
 *   dependency or application-specific analysis code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named test benchmark direction values accepted by this public contract.
 */
typedef enum UmiTestBenchmarkDirection {
    UMI_TEST_BENCHMARK_LOWER_IS_BETTER = 1,
    UMI_TEST_BENCHMARK_HIGHER_IS_BETTER = 2
} UmiTestBenchmarkDirection;

/**
 * List the named test benchmark gate state values accepted by this public contract.
 */
typedef enum UmiTestBenchmarkGateState {
    UMI_TEST_BENCHMARK_GATE_UNKNOWN = 0,
    UMI_TEST_BENCHMARK_GATE_PASSED = 1,
    UMI_TEST_BENCHMARK_GATE_IMPROVED = 2,
    UMI_TEST_BENCHMARK_GATE_REGRESSED = 3
} UmiTestBenchmarkGateState;

/**
 * Represent the test benchmark policy data shared with callers of this public contract.
 */
typedef struct UmiTestBenchmarkPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    size_t minimum_sample_count;
    double regression_tolerance_percent;
    double improvement_threshold_percent;
    UmiTestBenchmarkDirection direction;
    int honour_provider_regression;
} UmiTestBenchmarkPolicy;

/**
 * Represent the test benchmark metric analysis data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the test benchmark analysis snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the test benchmark analysis data shared with callers of this public contract.
 */
typedef struct UmiTestBenchmarkAnalysis UmiTestBenchmarkAnalysis;

/**
 * Initialise test benchmark policy from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_benchmark_policy_init(UmiTestBenchmarkPolicy *policy);
/**
 * Initialise test benchmark analysis from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_benchmark_analysis_create(
    UmiTestBenchmarkAnalysis **out_analysis);
/**
 * Release or reset state held by test benchmark analysis so the same storage can be reused
 * safely.
 */
void umi_test_benchmark_analysis_destroy(UmiTestBenchmarkAnalysis *analysis);
/**
 * Provide the test benchmark analysis build operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_benchmark_analysis_build(
    UmiTestBenchmarkAnalysis *analysis,
    const UmiTestPlatformBenchmarkRegistry *benchmarks,
    const UmiTestBenchmarkPolicy *policy);
/**
 * Find test benchmark analysis while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_test_benchmark_analysis_at(
    const UmiTestBenchmarkAnalysis *analysis,
    size_t position,
    UmiTestBenchmarkMetricAnalysis *out_metric);
/**
 * Find test benchmark analysis while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_test_benchmark_analysis_find(
    const UmiTestBenchmarkAnalysis *analysis,
    const char *metric,
    const char *unit,
    UmiTestBenchmarkMetricAnalysis *out_metric);
/**
 * Provide the test benchmark analysis snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_benchmark_analysis_snapshot(
    const UmiTestBenchmarkAnalysis *analysis,
    UmiTestBenchmarkAnalysisSnapshot *out_snapshot);
/**
 * Return the number of records represented by test benchmark analysis without changing
 * their state.
 */
size_t umi_test_benchmark_analysis_count(
    const UmiTestBenchmarkAnalysis *analysis);
/**
 * Provide the test benchmark gate state text operation used by this module and its client
 * applications.
 */
const char *umi_test_benchmark_gate_state_text(UmiTestBenchmarkGateState state);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_BENCHMARK_ANALYSIS_H */
