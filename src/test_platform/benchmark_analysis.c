/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/benchmark_analysis.c
 *
 * PURPOSE:
 *   Implement dependency-free benchmark statistics and regression gates. The
 *   algorithms are deterministic and intentionally transparent for audit and
 *   teaching use.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/benchmark_analysis.h"

#include <stdlib.h>
#include <string.h>

#define BENCHMARK_METRIC_CAPACITY UMI_TEST_PLATFORM_BENCHMARK_CAPACITY

struct UmiTestBenchmarkAnalysis {
    UmiTestBenchmarkMetricAnalysis *metrics;
    size_t count;
    UmiTestBenchmarkAnalysisSnapshot snapshot;
    uint64_t revision;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static double absolute_value(double value)
{
    return value < 0.0 ? -value : value;
}

static double square_root(double value)
{
    double estimate;
    size_t iteration;
    if (value <= 0.0) return 0.0;
    estimate = value >= 1.0 ? value : 1.0;
    for (iteration = 0U; iteration < 32U; ++iteration) {
        const double next = 0.5 * (estimate + value / estimate);
        if (absolute_value(next - estimate) < 0.000000000001) return next;
        estimate = next;
    }
    return estimate;
}

static int compare_double(const void *left_value, const void *right_value)
{
    const double left = *(const double *)left_value;
    const double right = *(const double *)right_value;
    if (left < right) return -1;
    if (left > right) return 1;
    return 0;
}

static double percentile(const double *values, size_t count, uint32_t basis_points)
{
    uint64_t scaled;
    size_t lower;
    size_t upper;
    double fraction;
    if (values == NULL || count == 0U) return 0.0;
    if (count == 1U) return values[0];
    scaled = (uint64_t)(count - 1U) * (uint64_t)basis_points;
    lower = (size_t)(scaled / 10000U);
    upper = lower + 1U < count ? lower + 1U : lower;
    fraction = (double)(scaled % 10000U) / 10000.0;
    return values[lower] + (values[upper] - values[lower]) * fraction;
}

void umi_test_benchmark_policy_init(UmiTestBenchmarkPolicy *policy)
{
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->api_version = UMI_TEST_BENCHMARK_ANALYSIS_API_VERSION;
    policy->minimum_sample_count = 1U;
    policy->regression_tolerance_percent = 5.0;
    policy->improvement_threshold_percent = 5.0;
    policy->direction = UMI_TEST_BENCHMARK_LOWER_IS_BETTER;
    policy->honour_provider_regression = 1;
}

UmiStatus umi_test_benchmark_analysis_create(
    UmiTestBenchmarkAnalysis **out_analysis)
{
    UmiTestBenchmarkAnalysis *analysis;
    if (out_analysis == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_analysis = NULL;
    analysis = (UmiTestBenchmarkAnalysis *)calloc(1U, sizeof(*analysis));
    if (analysis == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    analysis->metrics = (UmiTestBenchmarkMetricAnalysis *)calloc(
        BENCHMARK_METRIC_CAPACITY, sizeof(analysis->metrics[0]));
    if (analysis->metrics == NULL) {
        free(analysis);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    analysis->revision = 1U;
    *out_analysis = analysis;
    return UMI_STATUS_OK;
}

void umi_test_benchmark_analysis_destroy(UmiTestBenchmarkAnalysis *analysis)
{
    if (analysis == NULL) return;
    free(analysis->metrics);
    free(analysis);
}

static size_t find_metric(const UmiTestBenchmarkAnalysis *analysis,
                          const char *metric,
                          const char *unit)
{
    size_t index;
    for (index = 0U; index < analysis->count; ++index) {
        if (strcmp(analysis->metrics[index].metric, metric) == 0 &&
            strcmp(analysis->metrics[index].unit, unit) == 0) {
            return index;
        }
    }
    return (size_t)-1;
}

static UmiTestBenchmarkGateState evaluate_metric(
    double delta_percent,
    size_t sample_count,
    size_t provider_regressions,
    const UmiTestBenchmarkPolicy *policy,
    int has_baseline)
{
    double directional_delta = delta_percent;
    if (sample_count < policy->minimum_sample_count) {
        return UMI_TEST_BENCHMARK_GATE_UNKNOWN;
    }
    if (policy->honour_provider_regression && provider_regressions > 0U) {
        return UMI_TEST_BENCHMARK_GATE_REGRESSED;
    }
    if (!has_baseline) return UMI_TEST_BENCHMARK_GATE_UNKNOWN;
    if (policy->direction == UMI_TEST_BENCHMARK_HIGHER_IS_BETTER) {
        directional_delta = -directional_delta;
    }
    if (directional_delta > policy->regression_tolerance_percent) {
        return UMI_TEST_BENCHMARK_GATE_REGRESSED;
    }
    if (directional_delta < -policy->improvement_threshold_percent) {
        return UMI_TEST_BENCHMARK_GATE_IMPROVED;
    }
    return UMI_TEST_BENCHMARK_GATE_PASSED;
}

static UmiStatus analyse_metric(
    UmiTestBenchmarkMetricAnalysis *metric_analysis,
    const UmiTestPlatformBenchmarkRegistry *benchmarks,
    const UmiTestBenchmarkPolicy *policy,
    double *values,
    size_t value_capacity)
{
    size_t index;
    size_t count = 0U;
    size_t baseline_count = 0U;
    double baseline_total = 0.0;
    double sum = 0.0;
    double variance_sum = 0.0;

    for (index = 0U;
         index < umi_test_platform_benchmark_registry_count(benchmarks);
         ++index) {
        UmiTestPlatformBenchmarkSnapshot sample;
        UmiStatus status = umi_test_platform_benchmark_registry_at(
            benchmarks, index, &sample);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(sample.metric, metric_analysis->metric) != 0 ||
            strcmp(sample.unit, metric_analysis->unit) != 0) continue;
        if (count >= value_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        values[count++] = sample.value;
        sum += sample.value;
        if (sample.baseline != 0.0) {
            baseline_total += sample.baseline;
            ++baseline_count;
        }
        if (sample.regression) ++metric_analysis->provider_regression_count;
    }
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    qsort(values, count, sizeof(values[0]), compare_double);
    metric_analysis->sample_count = count;
    metric_analysis->minimum = values[0];
    metric_analysis->maximum = values[count - 1U];
    metric_analysis->mean = sum / (double)count;
    metric_analysis->median = percentile(values, count, 5000U);
    metric_analysis->percentile_90 = percentile(values, count, 9000U);
    metric_analysis->percentile_95 = percentile(values, count, 9500U);
    metric_analysis->percentile_99 = percentile(values, count, 9900U);
    for (index = 0U; index < count; ++index) {
        const double difference = values[index] - metric_analysis->mean;
        variance_sum += difference * difference;
    }
    metric_analysis->standard_deviation = square_root(variance_sum / (double)count);
    if (baseline_count > 0U) {
        metric_analysis->baseline = baseline_total / (double)baseline_count;
        if (metric_analysis->baseline != 0.0) {
            metric_analysis->delta_percent =
                ((metric_analysis->mean - metric_analysis->baseline) /
                 absolute_value(metric_analysis->baseline)) * 100.0;
        }
    }
    metric_analysis->gate_state = evaluate_metric(
        metric_analysis->delta_percent, count,
        metric_analysis->provider_regression_count, policy,
        baseline_count > 0U);
    return UMI_STATUS_OK;
}

static int compare_metric(const void *left_value, const void *right_value)
{
    const UmiTestBenchmarkMetricAnalysis *left =
        (const UmiTestBenchmarkMetricAnalysis *)left_value;
    const UmiTestBenchmarkMetricAnalysis *right =
        (const UmiTestBenchmarkMetricAnalysis *)right_value;
    if (left->gate_state > right->gate_state) return -1;
    if (left->gate_state < right->gate_state) return 1;
    {
        const int metric_order = strcmp(left->metric, right->metric);
        return metric_order != 0 ? metric_order : strcmp(left->unit, right->unit);
    }
}

UmiStatus umi_test_benchmark_analysis_build(
    UmiTestBenchmarkAnalysis *analysis,
    const UmiTestPlatformBenchmarkRegistry *benchmarks,
    const UmiTestBenchmarkPolicy *policy)
{
    UmiTestBenchmarkPolicy effective_policy;
    double *values;
    size_t index;

    if (analysis == NULL || benchmarks == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (policy == NULL) {
        umi_test_benchmark_policy_init(&effective_policy);
        policy = &effective_policy;
    }
    if (policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_TEST_BENCHMARK_ANALYSIS_API_VERSION ||
        policy->direction < UMI_TEST_BENCHMARK_LOWER_IS_BETTER ||
        policy->direction > UMI_TEST_BENCHMARK_HIGHER_IS_BETTER ||
        policy->regression_tolerance_percent < 0.0 ||
        policy->improvement_threshold_percent < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    values = (double *)calloc(BENCHMARK_METRIC_CAPACITY, sizeof(values[0]));
    if (values == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    analysis->count = 0U;
    (void)memset(&analysis->snapshot, 0, sizeof(analysis->snapshot));

    for (index = 0U;
         index < umi_test_platform_benchmark_registry_count(benchmarks);
         ++index) {
        UmiTestPlatformBenchmarkSnapshot sample;
        UmiStatus status = umi_test_platform_benchmark_registry_at(
            benchmarks, index, &sample);
        size_t metric_index;
        if (status != UMI_STATUS_OK) {
            free(values);
            return status;
        }
        if (sample.metric[0] == '\0') continue;
        metric_index = find_metric(analysis, sample.metric, sample.unit);
        if (metric_index == (size_t)-1) {
            UmiTestBenchmarkMetricAnalysis *metric;
            if (analysis->count >= BENCHMARK_METRIC_CAPACITY) {
                free(values);
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            metric_index = analysis->count++;
            metric = &analysis->metrics[metric_index];
            (void)memset(metric, 0, sizeof(*metric));
            metric->struct_size = (uint32_t)sizeof(*metric);
            metric->api_version = UMI_TEST_BENCHMARK_ANALYSIS_API_VERSION;
            copy_text(metric->metric, sizeof(metric->metric), sample.metric);
            copy_text(metric->unit, sizeof(metric->unit), sample.unit);
        }
    }

    for (index = 0U; index < analysis->count; ++index) {
        UmiStatus status = analyse_metric(&analysis->metrics[index], benchmarks,
                                          policy, values,
                                          BENCHMARK_METRIC_CAPACITY);
        if (status != UMI_STATUS_OK) {
            free(values);
            return status;
        }
    }
    free(values);
    qsort(analysis->metrics, analysis->count, sizeof(analysis->metrics[0]),
          compare_metric);

    analysis->snapshot.struct_size = (uint32_t)sizeof(analysis->snapshot);
    analysis->snapshot.api_version = UMI_TEST_BENCHMARK_ANALYSIS_API_VERSION;
    analysis->snapshot.metric_count = analysis->count;
    analysis->snapshot.source_revision =
        umi_test_platform_benchmark_registry_revision(benchmarks);
    for (index = 0U; index < analysis->count; ++index) {
        const UmiTestBenchmarkMetricAnalysis *metric = &analysis->metrics[index];
        analysis->snapshot.sample_count += metric->sample_count;
        switch (metric->gate_state) {
            case UMI_TEST_BENCHMARK_GATE_PASSED:
                ++analysis->snapshot.passed_metric_count;
                break;
            case UMI_TEST_BENCHMARK_GATE_IMPROVED:
                ++analysis->snapshot.improved_metric_count;
                break;
            case UMI_TEST_BENCHMARK_GATE_REGRESSED:
                ++analysis->snapshot.regressed_metric_count;
                break;
            case UMI_TEST_BENCHMARK_GATE_UNKNOWN:
            default:
                break;
        }
    }
    analysis->snapshot.revision = ++analysis->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_benchmark_analysis_at(
    const UmiTestBenchmarkAnalysis *analysis,
    size_t position,
    UmiTestBenchmarkMetricAnalysis *out_metric)
{
    if (analysis == NULL || out_metric == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= analysis->count) return UMI_STATUS_NOT_FOUND;
    *out_metric = analysis->metrics[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_test_benchmark_analysis_find(
    const UmiTestBenchmarkAnalysis *analysis,
    const char *metric,
    const char *unit,
    UmiTestBenchmarkMetricAnalysis *out_metric)
{
    size_t position;
    if (analysis == NULL || metric == NULL || unit == NULL || out_metric == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_metric(analysis, metric, unit);
    if (position == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    *out_metric = analysis->metrics[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_test_benchmark_analysis_snapshot(
    const UmiTestBenchmarkAnalysis *analysis,
    UmiTestBenchmarkAnalysisSnapshot *out_snapshot)
{
    if (analysis == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = analysis->snapshot;
    return UMI_STATUS_OK;
}

size_t umi_test_benchmark_analysis_count(
    const UmiTestBenchmarkAnalysis *analysis)
{
    return analysis != NULL ? analysis->count : 0U;
}

const char *umi_test_benchmark_gate_state_text(UmiTestBenchmarkGateState state)
{
    switch (state) {
        case UMI_TEST_BENCHMARK_GATE_PASSED: return "Passed";
        case UMI_TEST_BENCHMARK_GATE_IMPROVED: return "Improved";
        case UMI_TEST_BENCHMARK_GATE_REGRESSED: return "Regressed";
        case UMI_TEST_BENCHMARK_GATE_UNKNOWN:
        default: return "Unknown";
    }
}
