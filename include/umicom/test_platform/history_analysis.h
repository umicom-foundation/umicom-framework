/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/history_analysis.h
 *
 * PURPOSE:
 *   Turn retained test results into stability, flakiness, failure-streak and
 *   duration-trend evidence suitable for Test Explorer and quality gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_HISTORY_ANALYSIS_H
#define UMICOM_TEST_PLATFORM_HISTORY_ANALYSIS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/test_platform/item.h"
#include "umicom/test_platform/result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_HISTORY_ANALYSIS_API_VERSION 1U

/**
 * List the named test stability state values accepted by this public contract.
 */
typedef enum UmiTestStabilityState {
    UMI_TEST_STABILITY_UNKNOWN = 0,
    UMI_TEST_STABILITY_STABLE = 1,
    UMI_TEST_STABILITY_FLAKY = 2,
    UMI_TEST_STABILITY_FAILING = 3,
    UMI_TEST_STABILITY_SLOW = 4,
    UMI_TEST_STABILITY_REGRESSED = 5
} UmiTestStabilityState;

/**
 * Represent the test history policy data shared with callers of this public contract.
 */
typedef struct UmiTestHistoryPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    size_t minimum_samples;
    uint32_t flaky_failure_ratio_basis_points;
    size_t failing_streak_threshold;
    double slow_duration_ms;
    double duration_regression_percent;
} UmiTestHistoryPolicy;

/**
 * Represent the test history item analysis data shared with callers of this public
 * contract.
 */
typedef struct UmiTestHistoryItemAnalysis {
    uint32_t struct_size;
    uint32_t api_version;
    char item_id[128];
    char item_name[256];
    size_t sample_count;
    size_t passed_count;
    size_t failed_count;
    size_t skipped_count;
    size_t cancelled_count;
    size_t timed_out_count;
    size_t flaky_result_count;
    size_t current_failure_streak;
    size_t longest_failure_streak;
    uint32_t pass_rate_basis_points;
    double latest_duration_ms;
    double minimum_duration_ms;
    double maximum_duration_ms;
    double average_duration_ms;
    double duration_trend_percent;
    UmiTestPlatformOutcome latest_outcome;
    UmiTestStabilityState stability;
    uint64_t latest_sequence;
} UmiTestHistoryItemAnalysis;

/**
 * Represent the test history analysis snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestHistoryAnalysisSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t item_count;
    size_t analysed_item_count;
    size_t stable_count;
    size_t flaky_count;
    size_t failing_count;
    size_t slow_count;
    size_t regressed_count;
    uint64_t item_revision;
    uint64_t result_revision;
    uint64_t revision;
} UmiTestHistoryAnalysisSnapshot;

/**
 * Represent the test history analysis data shared with callers of this public contract.
 */
typedef struct UmiTestHistoryAnalysis UmiTestHistoryAnalysis;

/**
 * Initialise test history policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_history_policy_init(UmiTestHistoryPolicy *policy);
/**
 * Initialise test history analysis from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_history_analysis_create(
    UmiTestHistoryAnalysis **out_analysis);
/**
 * Release or reset state held by test history analysis so the same storage can be reused
 * safely.
 */
void umi_test_history_analysis_destroy(UmiTestHistoryAnalysis *analysis);
/**
 * Provide the test history analysis build operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_history_analysis_build(
    UmiTestHistoryAnalysis *analysis,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestHistoryPolicy *policy);
/**
 * Find test history analysis while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_test_history_analysis_at(
    const UmiTestHistoryAnalysis *analysis,
    size_t position,
    UmiTestHistoryItemAnalysis *out_item);
/**
 * Find test history analysis while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_test_history_analysis_find(
    const UmiTestHistoryAnalysis *analysis,
    const char *item_id,
    UmiTestHistoryItemAnalysis *out_item);
/**
 * Provide the test history analysis snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_history_analysis_snapshot(
    const UmiTestHistoryAnalysis *analysis,
    UmiTestHistoryAnalysisSnapshot *out_snapshot);
/**
 * Return the number of records represented by test history analysis without changing their
 * state.
 */
size_t umi_test_history_analysis_count(const UmiTestHistoryAnalysis *analysis);
/**
 * Provide the test stability state text operation used by this module and its client
 * applications.
 */
const char *umi_test_stability_state_text(UmiTestStabilityState state);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_HISTORY_ANALYSIS_H */
