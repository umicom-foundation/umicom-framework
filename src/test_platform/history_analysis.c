/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/history_analysis.c
 *
 * PURPOSE:
 *   Implement retained-result analytics for flakiness, stability, failure
 *   streaks and duration trends. The analyser never mutates canonical results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/history_analysis.h"

#include <stdlib.h>
#include <string.h>

#define HISTORY_ITEM_CAPACITY UMI_TEST_PLATFORM_ITEM_CAPACITY
#define HISTORY_RESULT_CAPACITY UMI_TEST_PLATFORM_RESULT_CAPACITY

struct UmiTestHistoryAnalysis {
    UmiTestHistoryItemAnalysis *items;
    size_t count;
    UmiTestHistoryAnalysisSnapshot snapshot;
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

static int compare_result_sequence(const void *left_value,
                                   const void *right_value)
{
    const UmiTestPlatformResultSnapshot *left =
        (const UmiTestPlatformResultSnapshot *)left_value;
    const UmiTestPlatformResultSnapshot *right =
        (const UmiTestPlatformResultSnapshot *)right_value;
    if (left->sequence < right->sequence) return -1;
    if (left->sequence > right->sequence) return 1;
    if (left->revision < right->revision) return -1;
    if (left->revision > right->revision) return 1;
    return strcmp(left->id, right->id);
}

static int compare_analysis(const void *left_value, const void *right_value)
{
    const UmiTestHistoryItemAnalysis *left =
        (const UmiTestHistoryItemAnalysis *)left_value;
    const UmiTestHistoryItemAnalysis *right =
        (const UmiTestHistoryItemAnalysis *)right_value;
    if (left->stability > right->stability) return -1;
    if (left->stability < right->stability) return 1;
    if (left->current_failure_streak > right->current_failure_streak) return -1;
    if (left->current_failure_streak < right->current_failure_streak) return 1;
    return strcmp(left->item_id, right->item_id);
}

void umi_test_history_policy_init(UmiTestHistoryPolicy *policy)
{
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->api_version = UMI_TEST_HISTORY_ANALYSIS_API_VERSION;
    policy->minimum_samples = 3U;
    policy->flaky_failure_ratio_basis_points = 1000U;
    policy->failing_streak_threshold = 2U;
    policy->slow_duration_ms = 1000.0;
    policy->duration_regression_percent = 25.0;
}

UmiStatus umi_test_history_analysis_create(
    UmiTestHistoryAnalysis **out_analysis)
{
    UmiTestHistoryAnalysis *analysis;
    if (out_analysis == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_analysis = NULL;
    analysis = (UmiTestHistoryAnalysis *)calloc(1U, sizeof(*analysis));
    if (analysis == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    analysis->items = (UmiTestHistoryItemAnalysis *)calloc(
        HISTORY_ITEM_CAPACITY, sizeof(analysis->items[0]));
    if (analysis->items == NULL) {
        free(analysis);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    analysis->revision = 1U;
    *out_analysis = analysis;
    return UMI_STATUS_OK;
}

void umi_test_history_analysis_destroy(UmiTestHistoryAnalysis *analysis)
{
    if (analysis == NULL) return;
    free(analysis->items);
    free(analysis);
}

static size_t collect_results(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformResultSnapshot *buffer,
    size_t capacity,
    UmiStatus *out_status)
{
    size_t index;
    size_t count = 0U;
    *out_status = UMI_STATUS_OK;
    for (index = 0U;
         index < umi_test_platform_result_registry_count(results);
         ++index) {
        UmiTestPlatformResultSnapshot result;
        UmiStatus status = umi_test_platform_result_registry_at(results, index,
                                                                 &result);
        if (status != UMI_STATUS_OK) {
            *out_status = status;
            return 0U;
        }
        if (strcmp(result.item_id, item_id) != 0) continue;
        if (count >= capacity) {
            *out_status = UMI_STATUS_CAPACITY_EXCEEDED;
            return 0U;
        }
        buffer[count++] = result;
    }
    qsort(buffer, count, sizeof(buffer[0]), compare_result_sequence);
    return count;
}

static uint32_t ratio_basis_points(size_t numerator, size_t denominator)
{
    if (denominator == 0U) return 0U;
    if (numerator >= denominator) return 10000U;
    return (uint32_t)(((uint64_t)numerator * 10000U) /
                      (uint64_t)denominator);
}

static void calculate_duration_trend(
    const UmiTestPlatformResultSnapshot *results,
    size_t count,
    double *out_trend)
{
    size_t midpoint;
    size_t index;
    double older_total = 0.0;
    double newer_total = 0.0;
    size_t older_count = 0U;
    size_t newer_count = 0U;

    *out_trend = 0.0;
    if (count < 2U) return;
    midpoint = count / 2U;
    for (index = 0U; index < count; ++index) {
        if (index < midpoint) {
            older_total += results[index].duration_ms;
            ++older_count;
        } else {
            newer_total += results[index].duration_ms;
            ++newer_count;
        }
    }
    if (older_count > 0U && newer_count > 0U) {
        const double older_average = older_total / (double)older_count;
        const double newer_average = newer_total / (double)newer_count;
        if (absolute_value(older_average) > 0.000000001) {
            *out_trend = ((newer_average - older_average) /
                          absolute_value(older_average)) * 100.0;
        }
    }
}

static void analyse_item(
    UmiTestHistoryItemAnalysis *analysis,
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformResultSnapshot *results,
    size_t count,
    const UmiTestHistoryPolicy *policy)
{
    size_t index;
    size_t running_failure_streak = 0U;
    size_t first_pass_or_failure = 0U;
    double duration_total = 0.0;
    int mixed_outcomes = 0;

    (void)memset(analysis, 0, sizeof(*analysis));
    analysis->struct_size = (uint32_t)sizeof(*analysis);
    analysis->api_version = UMI_TEST_HISTORY_ANALYSIS_API_VERSION;
    copy_text(analysis->item_id, sizeof(analysis->item_id), item->id);
    copy_text(analysis->item_name, sizeof(analysis->item_name), item->name);
    analysis->sample_count = count;
    analysis->stability = UMI_TEST_STABILITY_UNKNOWN;

    for (index = 0U; index < count; ++index) {
        const UmiTestPlatformOutcome outcome =
            (UmiTestPlatformOutcome)results[index].outcome;
        duration_total += results[index].duration_ms;
        if (index == 0U || results[index].duration_ms < analysis->minimum_duration_ms) {
            analysis->minimum_duration_ms = results[index].duration_ms;
        }
        if (results[index].duration_ms > analysis->maximum_duration_ms) {
            analysis->maximum_duration_ms = results[index].duration_ms;
        }
        if (results[index].flaky) ++analysis->flaky_result_count;
        switch (outcome) {
            case UMI_TEST_PLATFORM_OUTCOME_PASSED:
                ++analysis->passed_count;
                running_failure_streak = 0U;
                break;
            case UMI_TEST_PLATFORM_OUTCOME_FAILED:
                ++analysis->failed_count;
                ++running_failure_streak;
                break;
            case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT:
                ++analysis->timed_out_count;
                ++running_failure_streak;
                break;
            case UMI_TEST_PLATFORM_OUTCOME_SKIPPED:
                ++analysis->skipped_count;
                running_failure_streak = 0U;
                break;
            case UMI_TEST_PLATFORM_OUTCOME_CANCELLED:
                ++analysis->cancelled_count;
                running_failure_streak = 0U;
                break;
            case UMI_TEST_PLATFORM_OUTCOME_NOT_RUN:
            default:
                running_failure_streak = 0U;
                break;
        }
        if (running_failure_streak > analysis->longest_failure_streak) {
            analysis->longest_failure_streak = running_failure_streak;
        }
    }
    if (count > 0U) {
        const UmiTestPlatformResultSnapshot *latest = &results[count - 1U];
        analysis->latest_outcome = (UmiTestPlatformOutcome)latest->outcome;
        analysis->latest_duration_ms = latest->duration_ms;
        analysis->latest_sequence = latest->sequence;
        analysis->average_duration_ms = duration_total / (double)count;
    }
    index = count;
    while (index > 0U) {
        const UmiTestPlatformOutcome outcome =
            (UmiTestPlatformOutcome)results[index - 1U].outcome;
        if (outcome != UMI_TEST_PLATFORM_OUTCOME_FAILED &&
            outcome != UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) break;
        ++analysis->current_failure_streak;
        --index;
    }
    first_pass_or_failure = analysis->passed_count + analysis->failed_count +
                            analysis->timed_out_count;
    analysis->pass_rate_basis_points = ratio_basis_points(
        analysis->passed_count, first_pass_or_failure);
    calculate_duration_trend(results, count, &analysis->duration_trend_percent);
    mixed_outcomes = analysis->passed_count > 0U &&
        (analysis->failed_count + analysis->timed_out_count) > 0U;

    if (count < policy->minimum_samples) {
        analysis->stability = UMI_TEST_STABILITY_UNKNOWN;
    } else if (analysis->current_failure_streak >=
               policy->failing_streak_threshold) {
        analysis->stability = UMI_TEST_STABILITY_FAILING;
    } else if (analysis->flaky_result_count > 0U ||
               (mixed_outcomes &&
                ratio_basis_points(analysis->failed_count +
                                       analysis->timed_out_count,
                                   first_pass_or_failure) >=
                    policy->flaky_failure_ratio_basis_points)) {
        analysis->stability = UMI_TEST_STABILITY_FLAKY;
    } else if (analysis->duration_trend_percent >=
               policy->duration_regression_percent) {
        analysis->stability = UMI_TEST_STABILITY_REGRESSED;
    } else if (analysis->average_duration_ms >= policy->slow_duration_ms &&
               policy->slow_duration_ms > 0.0) {
        analysis->stability = UMI_TEST_STABILITY_SLOW;
    } else {
        analysis->stability = UMI_TEST_STABILITY_STABLE;
    }
}

UmiStatus umi_test_history_analysis_build(
    UmiTestHistoryAnalysis *analysis,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestHistoryPolicy *policy)
{
    UmiTestHistoryPolicy effective_policy;
    UmiTestPlatformResultSnapshot *buffer;
    size_t item_index;

    if (analysis == NULL || items == NULL || results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (policy == NULL) {
        umi_test_history_policy_init(&effective_policy);
        policy = &effective_policy;
    }
    if (policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_TEST_HISTORY_ANALYSIS_API_VERSION ||
        policy->flaky_failure_ratio_basis_points > 10000U ||
        policy->failing_streak_threshold == 0U ||
        policy->slow_duration_ms < 0.0 ||
        policy->duration_regression_percent < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    buffer = (UmiTestPlatformResultSnapshot *)calloc(
        HISTORY_RESULT_CAPACITY, sizeof(buffer[0]));
    if (buffer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    analysis->count = 0U;
    (void)memset(&analysis->snapshot, 0, sizeof(analysis->snapshot));

    for (item_index = 0U;
         item_index < umi_test_platform_item_registry_count(items);
         ++item_index) {
        UmiTestPlatformItemSnapshot item;
        UmiStatus status;
        size_t sample_count;
        if (analysis->count >= HISTORY_ITEM_CAPACITY) {
            free(buffer);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_test_platform_item_registry_at(items, item_index, &item);
        if (status != UMI_STATUS_OK) {
            free(buffer);
            return status;
        }
        sample_count = collect_results(results, item.id, buffer,
                                       HISTORY_RESULT_CAPACITY, &status);
        if (status != UMI_STATUS_OK) {
            free(buffer);
            return status;
        }
        analyse_item(&analysis->items[analysis->count++], &item, buffer,
                     sample_count, policy);
    }
    free(buffer);
    qsort(analysis->items, analysis->count, sizeof(analysis->items[0]),
          compare_analysis);

    analysis->snapshot.struct_size = (uint32_t)sizeof(analysis->snapshot);
    analysis->snapshot.api_version = UMI_TEST_HISTORY_ANALYSIS_API_VERSION;
    analysis->snapshot.item_count = analysis->count;
    analysis->snapshot.item_revision =
        umi_test_platform_item_registry_revision(items);
    analysis->snapshot.result_revision =
        umi_test_platform_result_registry_revision(results);
    for (item_index = 0U; item_index < analysis->count; ++item_index) {
        const UmiTestStabilityState stability = analysis->items[item_index].stability;
        if (analysis->items[item_index].sample_count > 0U) {
            ++analysis->snapshot.analysed_item_count;
        }
        switch (stability) {
            case UMI_TEST_STABILITY_STABLE:
                ++analysis->snapshot.stable_count;
                break;
            case UMI_TEST_STABILITY_FLAKY:
                ++analysis->snapshot.flaky_count;
                break;
            case UMI_TEST_STABILITY_FAILING:
                ++analysis->snapshot.failing_count;
                break;
            case UMI_TEST_STABILITY_SLOW:
                ++analysis->snapshot.slow_count;
                break;
            case UMI_TEST_STABILITY_REGRESSED:
                ++analysis->snapshot.regressed_count;
                break;
            case UMI_TEST_STABILITY_UNKNOWN:
            default:
                break;
        }
    }
    analysis->snapshot.revision = ++analysis->revision;
    return UMI_STATUS_OK;
}

static size_t find_item(const UmiTestHistoryAnalysis *analysis,
                        const char *item_id)
{
    size_t index;
    for (index = 0U; index < analysis->count; ++index) {
        if (strcmp(analysis->items[index].item_id, item_id) == 0) return index;
    }
    return (size_t)-1;
}

UmiStatus umi_test_history_analysis_at(
    const UmiTestHistoryAnalysis *analysis,
    size_t position,
    UmiTestHistoryItemAnalysis *out_item)
{
    if (analysis == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= analysis->count) return UMI_STATUS_NOT_FOUND;
    *out_item = analysis->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_test_history_analysis_find(
    const UmiTestHistoryAnalysis *analysis,
    const char *item_id,
    UmiTestHistoryItemAnalysis *out_item)
{
    size_t position;
    if (analysis == NULL || item_id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_item(analysis, item_id);
    if (position == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    *out_item = analysis->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_test_history_analysis_snapshot(
    const UmiTestHistoryAnalysis *analysis,
    UmiTestHistoryAnalysisSnapshot *out_snapshot)
{
    if (analysis == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = analysis->snapshot;
    return UMI_STATUS_OK;
}

size_t umi_test_history_analysis_count(const UmiTestHistoryAnalysis *analysis)
{
    return analysis != NULL ? analysis->count : 0U;
}

const char *umi_test_stability_state_text(UmiTestStabilityState state)
{
    switch (state) {
        case UMI_TEST_STABILITY_STABLE: return "Stable";
        case UMI_TEST_STABILITY_FLAKY: return "Flaky";
        case UMI_TEST_STABILITY_FAILING: return "Failing";
        case UMI_TEST_STABILITY_SLOW: return "Slow";
        case UMI_TEST_STABILITY_REGRESSED: return "Regressed";
        case UMI_TEST_STABILITY_UNKNOWN:
        default: return "Unknown";
    }
}
