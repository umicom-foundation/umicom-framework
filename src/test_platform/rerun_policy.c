/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/rerun_policy.c
 *
 * PURPOSE:
 *   Implement evidence-based rerun planning. Candidates are ranked using their
 *   latest outcome, explicit flakiness, failure streak and duration so every
 *   frontend receives the same deterministic plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/rerun_policy.h"

#include <stdlib.h>
#include <string.h>

struct UmiTestRerunPlan {
    UmiTestRerunCandidate candidates[UMI_TEST_PLATFORM_SELECTION_CAPACITY];
    size_t count;
    uint64_t item_revision;
    uint64_t result_revision;
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

static int compare_candidate(const void *left_value, const void *right_value)
{
    const UmiTestRerunCandidate *left =
        (const UmiTestRerunCandidate *)left_value;
    const UmiTestRerunCandidate *right =
        (const UmiTestRerunCandidate *)right_value;
    if (left->score > right->score) return -1;
    if (left->score < right->score) return 1;
    return strcmp(left->item_id, right->item_id);
}

void umi_test_rerun_policy_init(UmiTestRerunPolicy *policy)
{
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->api_version = UMI_TEST_RERUN_POLICY_API_VERSION;
    policy->reason_mask =
        (uint32_t)UMI_TEST_RERUN_REASON_FAILED |
        (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT |
        (uint32_t)UMI_TEST_RERUN_REASON_FLAKY;
    policy->maximum_candidates = UMI_TEST_PLATFORM_SELECTION_CAPACITY;
    policy->minimum_failure_streak = 1U;
    policy->slow_duration_ms = 1000.0;
    policy->repeat_count = 1U;
}

UmiStatus umi_test_rerun_plan_create(UmiTestRerunPlan **out_plan)
{
    UmiTestRerunPlan *plan;
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_plan = NULL;
    plan = (UmiTestRerunPlan *)calloc(1U, sizeof(*plan));
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->revision = 1U;
    *out_plan = plan;
    return UMI_STATUS_OK;
}

void umi_test_rerun_plan_destroy(UmiTestRerunPlan *plan)
{
    free(plan);
}

static size_t collect_item_results(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformResultSnapshot *buffer,
    size_t capacity,
    UmiStatus *out_status)
{
    size_t index;
    size_t count = 0U;
    const size_t result_count = umi_test_platform_result_registry_count(results);
    *out_status = UMI_STATUS_OK;
    for (index = 0U; index < result_count; ++index) {
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

static uint32_t classify_candidate(
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformResultSnapshot *history,
    size_t sample_count,
    const UmiTestRerunPolicy *policy,
    size_t *out_failures,
    size_t *out_failure_streak,
    double *out_average_duration)
{
    size_t index;
    size_t failures = 0U;
    size_t passes = 0U;
    size_t streak = 0U;
    double duration_total = 0.0;
    uint32_t reasons = 0U;

    for (index = 0U; index < sample_count; ++index) {
        const UmiTestPlatformOutcome outcome =
            (UmiTestPlatformOutcome)history[index].outcome;
        duration_total += history[index].duration_ms;
        if (outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED ||
            outcome == UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) {
            ++failures;
        } else if (outcome == UMI_TEST_PLATFORM_OUTCOME_PASSED) {
            ++passes;
        }
        if (history[index].flaky) reasons |= (uint32_t)UMI_TEST_RERUN_REASON_FLAKY;
    }

    if (sample_count == 0U) {
        reasons |= (uint32_t)UMI_TEST_RERUN_REASON_NOT_RUN;
    } else {
        const UmiTestPlatformResultSnapshot *latest = &history[sample_count - 1U];
        const UmiTestPlatformOutcome latest_outcome =
            (UmiTestPlatformOutcome)latest->outcome;
        if (latest_outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_FAILED;
        } else if (latest_outcome == UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT;
        } else if (latest_outcome == UMI_TEST_PLATFORM_OUTCOME_SKIPPED) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_SKIPPED;
        }
        if (latest->duration_ms >= policy->slow_duration_ms &&
            policy->slow_duration_ms > 0.0) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_SLOW;
        }
        index = sample_count;
        while (index > 0U) {
            const UmiTestPlatformOutcome outcome =
                (UmiTestPlatformOutcome)history[index - 1U].outcome;
            if (outcome != UMI_TEST_PLATFORM_OUTCOME_FAILED &&
                outcome != UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) break;
            ++streak;
            --index;
        }
        if (failures > 0U && passes > 0U) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_FLAKY;
        }
    }

    if (!item->enabled && !policy->include_disabled) reasons = 0U;
    if (streak < policy->minimum_failure_streak &&
        (reasons & ((uint32_t)UMI_TEST_RERUN_REASON_FAILED |
                    (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT)) != 0U) {
        reasons &= ~((uint32_t)UMI_TEST_RERUN_REASON_FAILED |
                     (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT);
    }
    *out_failures = failures;
    *out_failure_streak = streak;
    *out_average_duration = sample_count > 0U
        ? duration_total / (double)sample_count
        : 0.0;
    return reasons;
}

static int32_t score_candidate(uint32_t reasons, size_t failure_streak,
                               double average_duration)
{
    int32_t score = 0;
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT) != 0U) score += 1200;
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FAILED) != 0U) score += 1000;
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FLAKY) != 0U) score += 700;
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_SLOW) != 0U) score += 400;
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_NOT_RUN) != 0U) score += 200;
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_SKIPPED) != 0U) score += 100;
    if (failure_streak > 100U) failure_streak = 100U;
    score += (int32_t)(failure_streak * 10U);
    if (average_duration > 100000.0) average_duration = 100000.0;
    score += (int32_t)(average_duration / 1000.0);
    return score;
}

UmiStatus umi_test_rerun_plan_build(
    UmiTestRerunPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestRerunPolicy *policy)
{
    UmiTestPlatformResultSnapshot *history;
    size_t item_index;
    size_t maximum;

    if (plan == NULL || items == NULL || results == NULL || policy == NULL ||
        policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_TEST_RERUN_POLICY_API_VERSION ||
        policy->repeat_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    maximum = policy->maximum_candidates;
    if (maximum == 0U || maximum > UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
        maximum = UMI_TEST_PLATFORM_SELECTION_CAPACITY;
    }
    history = (UmiTestPlatformResultSnapshot *)calloc(
        UMI_TEST_PLATFORM_RESULT_CAPACITY, sizeof(history[0]));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    plan->count = 0U;
    for (item_index = 0U;
         item_index < umi_test_platform_item_registry_count(items);
         ++item_index) {
        UmiTestPlatformItemSnapshot item;
        UmiTestRerunCandidate candidate;
        UmiStatus status;
        size_t sample_count;
        uint32_t reasons;

        status = umi_test_platform_item_registry_at(items, item_index, &item);
        if (status != UMI_STATUS_OK) {
            free(history);
            return status;
        }
        sample_count = collect_item_results(results, item.id, history,
                                            UMI_TEST_PLATFORM_RESULT_CAPACITY,
                                            &status);
        if (status != UMI_STATUS_OK) {
            free(history);
            return status;
        }
        (void)memset(&candidate, 0, sizeof(candidate));
        candidate.struct_size = (uint32_t)sizeof(candidate);
        candidate.api_version = UMI_TEST_RERUN_POLICY_API_VERSION;
        copy_text(candidate.item_id, sizeof(candidate.item_id), item.id);
        copy_text(candidate.item_name, sizeof(candidate.item_name), item.name);
        reasons = classify_candidate(
            &item, history, sample_count, policy, &candidate.failure_count,
            &candidate.failure_streak, &candidate.average_duration_ms);
        reasons &= policy->reason_mask;
        if (reasons == 0U) continue;
        candidate.reason_mask = reasons;
        candidate.sample_count = sample_count;
        if (sample_count > 0U) candidate.latest_result = history[sample_count - 1U];
        candidate.score = score_candidate(reasons, candidate.failure_streak,
                                          candidate.average_duration_ms);
        candidate.selected = 1;
        if (plan->count >= maximum) break;
        plan->candidates[plan->count++] = candidate;
    }
    free(history);

    qsort(plan->candidates, plan->count, sizeof(plan->candidates[0]),
          compare_candidate);
    plan->item_revision = umi_test_platform_item_registry_revision(items);
    plan->result_revision = umi_test_platform_result_registry_revision(results);
    ++plan->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_rerun_plan_set_selected(
    UmiTestRerunPlan *plan,
    const char *item_id,
    int selected)
{
    size_t index;
    if (plan == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < plan->count; ++index) {
        if (strcmp(plan->candidates[index].item_id, item_id) == 0) {
            plan->candidates[index].selected = selected != 0;
            ++plan->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_test_rerun_plan_select_all(UmiTestRerunPlan *plan, int selected)
{
    size_t index;
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < plan->count; ++index) {
        plan->candidates[index].selected = selected != 0;
    }
    ++plan->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_rerun_plan_selection(
    const UmiTestRerunPlan *plan,
    UmiTestPlatformSelection *out_selection)
{
    size_t index;
    if (plan == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_selection, 0, sizeof(*out_selection));
    for (index = 0U; index < plan->count; ++index) {
        if (!plan->candidates[index].selected) continue;
        if (out_selection->count >= UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        copy_text(out_selection->item_ids[out_selection->count],
                  sizeof(out_selection->item_ids[out_selection->count]),
                  plan->candidates[index].item_id);
        ++out_selection->count;
    }
    out_selection->source_revision = plan->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_rerun_plan_operation(
    const UmiTestRerunPlan *plan,
    const UmiTestRerunPolicy *policy,
    UmiTestPlatformOperationPlan *out_operation)
{
    UmiStatus status;
    if (plan == NULL || policy == NULL || out_operation == NULL ||
        policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_TEST_RERUN_POLICY_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_test_platform_operation_plan_init(out_operation,
                                          UMI_TEST_PLATFORM_OPERATION_REPEAT);
    status = umi_test_rerun_plan_selection(plan, &out_operation->selection);
    if (status != UMI_STATUS_OK) return status;
    if (out_operation->selection.count == 0U) return UMI_STATUS_NOT_FOUND;
    out_operation->repeat_count = policy->repeat_count;
    out_operation->stop_on_failure = policy->stop_on_failure;
    out_operation->generation = plan->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_rerun_plan_at(
    const UmiTestRerunPlan *plan,
    size_t position,
    UmiTestRerunCandidate *out_candidate)
{
    if (plan == NULL || out_candidate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= plan->count) return UMI_STATUS_NOT_FOUND;
    *out_candidate = plan->candidates[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_test_rerun_plan_snapshot(
    const UmiTestRerunPlan *plan,
    UmiTestRerunPlanSnapshot *out_snapshot)
{
    size_t index;
    if (plan == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TEST_RERUN_POLICY_API_VERSION;
    out_snapshot->candidate_count = plan->count;
    out_snapshot->item_revision = plan->item_revision;
    out_snapshot->result_revision = plan->result_revision;
    out_snapshot->revision = plan->revision;
    for (index = 0U; index < plan->count; ++index) {
        const uint32_t reasons = plan->candidates[index].reason_mask;
        if (plan->candidates[index].selected) ++out_snapshot->selected_count;
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FAILED) != 0U) {
            ++out_snapshot->failed_count;
        }
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT) != 0U) {
            ++out_snapshot->timed_out_count;
        }
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FLAKY) != 0U) {
            ++out_snapshot->flaky_count;
        }
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_SLOW) != 0U) {
            ++out_snapshot->slow_count;
        }
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_NOT_RUN) != 0U) {
            ++out_snapshot->not_run_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_test_rerun_plan_count(const UmiTestRerunPlan *plan)
{
    return plan != NULL ? plan->count : 0U;
}
