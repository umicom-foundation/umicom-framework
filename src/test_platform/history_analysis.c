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

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the absolute value operation used by this module and its client applications. */
static double absolute_value(double value)
{
    return value < 0.0 ? -value : value;
}

/*
 * Provide the compare result sequence operation used by this module and its client
 * applications.
 */
static int compare_result_sequence(const void *left_value,
                                   const void *right_value)
{
    const UmiTestPlatformResultSnapshot *left =
        (const UmiTestPlatformResultSnapshot *)left_value;
    const UmiTestPlatformResultSnapshot *right =
        (const UmiTestPlatformResultSnapshot *)right_value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence < right->sequence) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence > right->sequence) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->revision < right->revision) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->revision > right->revision) return 1;
    return strcmp(left->id, right->id);
}

/* Provide the compare analysis operation used by this module and its client applications. */
static int compare_analysis(const void *left_value, const void *right_value)
{
    const UmiTestHistoryItemAnalysis *left =
        (const UmiTestHistoryItemAnalysis *)left_value;
    const UmiTestHistoryItemAnalysis *right =
        (const UmiTestHistoryItemAnalysis *)right_value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->stability > right->stability) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->stability < right->stability) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->current_failure_streak > right->current_failure_streak) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->current_failure_streak < right->current_failure_streak) return 1;
    return strcmp(left->item_id, right->item_id);
}

/*
 * Initialise test history policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_history_policy_init(UmiTestHistoryPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Initialise test history analysis from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_history_analysis_create(
    UmiTestHistoryAnalysis **out_analysis)
{
    UmiTestHistoryAnalysis *analysis;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_analysis == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_analysis = NULL;
    analysis = (UmiTestHistoryAnalysis *)calloc(1U, sizeof(*analysis));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    analysis->items = (UmiTestHistoryItemAnalysis *)calloc(
        HISTORY_ITEM_CAPACITY, sizeof(analysis->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis->items == NULL) {
        free(analysis);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    analysis->revision = 1U;
    *out_analysis = analysis;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by test history analysis so the same storage can be reused
 * safely.
 */
void umi_test_history_analysis_destroy(UmiTestHistoryAnalysis *analysis)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL) return;
    free(analysis->items);
    free(analysis);
}

/* Provide the collect results operation used by this module and its client applications. */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_test_platform_result_registry_count(results);
         ++index) {
        UmiTestPlatformResultSnapshot result;
        UmiStatus status = umi_test_platform_result_registry_at(results, index,
                                                                 &result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            *out_status = status;
            return 0U;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (strcmp(result.item_id, item_id) != 0) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count >= capacity) {
            *out_status = UMI_STATUS_CAPACITY_EXCEEDED;
            return 0U;
        }
        buffer[count++] = result;
    }
    qsort(buffer, count, sizeof(buffer[0]), compare_result_sequence);
    return count;
}

/*
 * Provide the ratio basis points operation used by this module and its client
 * applications.
 */
static uint32_t ratio_basis_points(size_t numerator, size_t denominator)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (denominator == 0U) return 0U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (numerator >= denominator) return 10000U;
    return (uint32_t)(((uint64_t)numerator * 10000U) /
                      (uint64_t)denominator);
}

/*
 * Provide the calculate duration trend operation used by this module and its client
 * applications.
 */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count < 2U) return;
    midpoint = count / 2U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index < midpoint) {
            older_total += results[index].duration_ms;
            ++older_count;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            newer_total += results[index].duration_ms;
            ++newer_count;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (older_count > 0U && newer_count > 0U) {
        const double older_average = older_total / (double)older_count;
        const double newer_average = newer_total / (double)newer_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (absolute_value(older_average) > 0.000000001) {
            *out_trend = ((newer_average - older_average) /
                          absolute_value(older_average)) * 100.0;
        }
    }
}

/* Provide the analyse item operation used by this module and its client applications. */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const UmiTestPlatformOutcome outcome =
            (UmiTestPlatformOutcome)results[index].outcome;
        duration_total += results[index].duration_ms;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == 0U || results[index].duration_ms < analysis->minimum_duration_ms) {
            analysis->minimum_duration_ms = results[index].duration_ms;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (results[index].duration_ms > analysis->maximum_duration_ms) {
            analysis->maximum_duration_ms = results[index].duration_ms;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (results[index].flaky) ++analysis->flaky_result_count;
        /* Select the behaviour associated with the requested command or state value. */
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
        /* Apply this branch only when its contract condition is satisfied. */
        if (running_failure_streak > analysis->longest_failure_streak) {
            analysis->longest_failure_streak = running_failure_streak;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > 0U) {
        const UmiTestPlatformResultSnapshot *latest = &results[count - 1U];
        analysis->latest_outcome = (UmiTestPlatformOutcome)latest->outcome;
        analysis->latest_duration_ms = latest->duration_ms;
        analysis->latest_sequence = latest->sequence;
        analysis->average_duration_ms = duration_total / (double)count;
    }
    index = count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        const UmiTestPlatformOutcome outcome =
            (UmiTestPlatformOutcome)results[index - 1U].outcome;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count < policy->minimum_samples) {
        analysis->stability = UMI_TEST_STABILITY_UNKNOWN;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (analysis->current_failure_streak >=
               policy->failing_streak_threshold) {
        analysis->stability = UMI_TEST_STABILITY_FAILING;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (analysis->flaky_result_count > 0U ||
               (mixed_outcomes &&
                ratio_basis_points(analysis->failed_count +
                                       analysis->timed_out_count,
                                   first_pass_or_failure) >=
                    policy->flaky_failure_ratio_basis_points)) {
        analysis->stability = UMI_TEST_STABILITY_FLAKY;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (analysis->duration_trend_percent >=
               policy->duration_regression_percent) {
        analysis->stability = UMI_TEST_STABILITY_REGRESSED;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (analysis->average_duration_ms >= policy->slow_duration_ms &&
               policy->slow_duration_ms > 0.0) {
        analysis->stability = UMI_TEST_STABILITY_SLOW;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        analysis->stability = UMI_TEST_STABILITY_STABLE;
    }
}

/*
 * Provide the test history analysis build operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_history_analysis_build(
    UmiTestHistoryAnalysis *analysis,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestHistoryPolicy *policy)
{
    UmiTestHistoryPolicy effective_policy;
    UmiTestPlatformResultSnapshot *buffer;
    size_t item_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || items == NULL || results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) {
        umi_test_history_policy_init(&effective_policy);
        policy = &effective_policy;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    analysis->count = 0U;
    (void)memset(&analysis->snapshot, 0, sizeof(analysis->snapshot));

    /* Visit each bounded item once so every record receives the same rule. */
    for (item_index = 0U;
         item_index < umi_test_platform_item_registry_count(items);
         ++item_index) {
        UmiTestPlatformItemSnapshot item;
        UmiStatus status;
        size_t sample_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (analysis->count >= HISTORY_ITEM_CAPACITY) {
            free(buffer);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_test_platform_item_registry_at(items, item_index, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            free(buffer);
            return status;
        }
        sample_count = collect_results(results, item.id, buffer,
                                       HISTORY_RESULT_CAPACITY, &status);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (item_index = 0U; item_index < analysis->count; ++item_index) {
        const UmiTestStabilityState stability = analysis->items[item_index].stability;
        /* Apply this branch only when its contract condition is satisfied. */
        if (analysis->items[item_index].sample_count > 0U) {
            ++analysis->snapshot.analysed_item_count;
        }
        /* Select the behaviour associated with the requested command or state value. */
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

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiTestHistoryAnalysis *analysis,
                        const char *item_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < analysis->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(analysis->items[index].item_id, item_id) == 0) return index;
    }
    return (size_t)-1;
}

/*
 * Find test history analysis while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_test_history_analysis_at(
    const UmiTestHistoryAnalysis *analysis,
    size_t position,
    UmiTestHistoryItemAnalysis *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= analysis->count) return UMI_STATUS_NOT_FOUND;
    *out_item = analysis->items[position];
    return UMI_STATUS_OK;
}

/*
 * Find test history analysis while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_test_history_analysis_find(
    const UmiTestHistoryAnalysis *analysis,
    const char *item_id,
    UmiTestHistoryItemAnalysis *out_item)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || item_id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_item(analysis, item_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    *out_item = analysis->items[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the test history analysis snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_history_analysis_snapshot(
    const UmiTestHistoryAnalysis *analysis,
    UmiTestHistoryAnalysisSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = analysis->snapshot;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by test history analysis without changing their
 * state.
 */
size_t umi_test_history_analysis_count(const UmiTestHistoryAnalysis *analysis)
{
    return analysis != NULL ? analysis->count : 0U;
}

/*
 * Provide the test stability state text operation used by this module and its client
 * applications.
 */
const char *umi_test_stability_state_text(UmiTestStabilityState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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
