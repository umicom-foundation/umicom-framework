/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/rerun_policy.c
 *
 * PURPOSE:
 *   Implement evidence-based rerun planning. Candidates are ranked using their
 *   latest outcome, explicit flakiness, failure streak and duration so every
 *   frontend receives the same deterministic plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the compare candidate operation used by this module and its client applications. */
static int compare_candidate(const void *left_value, const void *right_value)
{
    const UmiTestRerunCandidate *left =
        (const UmiTestRerunCandidate *)left_value;
    const UmiTestRerunCandidate *right =
        (const UmiTestRerunCandidate *)right_value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score > right->score) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score < right->score) return 1;
    return strcmp(left->item_id, right->item_id);
}

/*
 * Initialise test rerun policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_rerun_policy_init(UmiTestRerunPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Initialise test rerun plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_rerun_plan_create(UmiTestRerunPlan **out_plan)
{
    UmiTestRerunPlan *plan;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_plan = NULL;
    plan = (UmiTestRerunPlan *)calloc(1U, sizeof(*plan));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->revision = 1U;
    *out_plan = plan;
    return UMI_STATUS_OK;
}

/* Release or reset state held by test rerun plan so the same storage can be reused safely. */
void umi_test_rerun_plan_destroy(UmiTestRerunPlan *plan)
{
    free(plan);
}

/*
 * Provide the collect item results operation used by this module and its client
 * applications.
 */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < result_count; ++index) {
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
 * Provide the classify candidate operation used by this module and its client
 * applications.
 */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sample_count; ++index) {
        const UmiTestPlatformOutcome outcome =
            (UmiTestPlatformOutcome)history[index].outcome;
        duration_total += history[index].duration_ms;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED ||
            outcome == UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) {
            ++failures;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (outcome == UMI_TEST_PLATFORM_OUTCOME_PASSED) {
            ++passes;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (history[index].flaky) reasons |= (uint32_t)UMI_TEST_RERUN_REASON_FLAKY;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (sample_count == 0U) {
        reasons |= (uint32_t)UMI_TEST_RERUN_REASON_NOT_RUN;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        const UmiTestPlatformResultSnapshot *latest = &history[sample_count - 1U];
        const UmiTestPlatformOutcome latest_outcome =
            (UmiTestPlatformOutcome)latest->outcome;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (latest_outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_FAILED;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (latest_outcome == UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (latest_outcome == UMI_TEST_PLATFORM_OUTCOME_SKIPPED) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_SKIPPED;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (latest->duration_ms >= policy->slow_duration_ms &&
            policy->slow_duration_ms > 0.0) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_SLOW;
        }
        index = sample_count;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (index > 0U) {
            const UmiTestPlatformOutcome outcome =
                (UmiTestPlatformOutcome)history[index - 1U].outcome;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (outcome != UMI_TEST_PLATFORM_OUTCOME_FAILED &&
                outcome != UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) break;
            ++streak;
            --index;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (failures > 0U && passes > 0U) {
            reasons |= (uint32_t)UMI_TEST_RERUN_REASON_FLAKY;
        }
    }

    /* Apply this operation only while the related capability or state is available. */
    if (!item->enabled && !policy->include_disabled) reasons = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
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

/* Provide the score candidate operation used by this module and its client applications. */
static int32_t score_candidate(uint32_t reasons, size_t failure_streak,
                               double average_duration)
{
    int32_t score = 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT) != 0U) score += 1200;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FAILED) != 0U) score += 1000;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FLAKY) != 0U) score += 700;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_SLOW) != 0U) score += 400;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_NOT_RUN) != 0U) score += 200;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_SKIPPED) != 0U) score += 100;
    /* Apply this branch only when its contract condition is satisfied. */
    if (failure_streak > 100U) failure_streak = 100U;
    score += (int32_t)(failure_streak * 10U);
    /* Apply this branch only when its contract condition is satisfied. */
    if (average_duration > 100000.0) average_duration = 100000.0;
    score += (int32_t)(average_duration / 1000.0);
    return score;
}

/*
 * Provide the test rerun plan build operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_rerun_plan_build(
    UmiTestRerunPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestRerunPolicy *policy)
{
    UmiTestPlatformResultSnapshot *history;
    size_t item_index;
    size_t maximum;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || items == NULL || results == NULL || policy == NULL ||
        policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_TEST_RERUN_POLICY_API_VERSION ||
        policy->repeat_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    maximum = policy->maximum_candidates;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (maximum == 0U || maximum > UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
        maximum = UMI_TEST_PLATFORM_SELECTION_CAPACITY;
    }
    history = (UmiTestPlatformResultSnapshot *)calloc(
        UMI_TEST_PLATFORM_RESULT_CAPACITY, sizeof(history[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    plan->count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (item_index = 0U;
         item_index < umi_test_platform_item_registry_count(items);
         ++item_index) {
        UmiTestPlatformItemSnapshot item;
        UmiTestRerunCandidate candidate;
        UmiStatus status;
        size_t sample_count;
        uint32_t reasons;

        status = umi_test_platform_item_registry_at(items, item_index, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            free(history);
            return status;
        }
        sample_count = collect_item_results(results, item.id, history,
                                            UMI_TEST_PLATFORM_RESULT_CAPACITY,
                                            &status);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
        /* Apply this branch only when its contract condition is satisfied. */
        if (reasons == 0U) continue;
        candidate.reason_mask = reasons;
        candidate.sample_count = sample_count;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (sample_count > 0U) candidate.latest_result = history[sample_count - 1U];
        candidate.score = score_candidate(reasons, candidate.failure_streak,
                                          candidate.average_duration_ms);
        candidate.selected = 1;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Find test rerun plan set while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_test_rerun_plan_set_selected(
    UmiTestRerunPlan *plan,
    const char *item_id,
    int selected)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(plan->candidates[index].item_id, item_id) == 0) {
            plan->candidates[index].selected = selected != 0;
            ++plan->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the test rerun plan select all operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_rerun_plan_select_all(UmiTestRerunPlan *plan, int selected)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->count; ++index) {
        plan->candidates[index].selected = selected != 0;
    }
    ++plan->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the test rerun plan selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_rerun_plan_selection(
    const UmiTestRerunPlan *plan,
    UmiTestPlatformSelection *out_selection)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_selection, 0, sizeof(*out_selection));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!plan->candidates[index].selected) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the test rerun plan operation operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_rerun_plan_operation(
    const UmiTestRerunPlan *plan,
    const UmiTestRerunPolicy *policy,
    UmiTestPlatformOperationPlan *out_operation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || policy == NULL || out_operation == NULL ||
        policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_TEST_RERUN_POLICY_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_test_platform_operation_plan_init(out_operation,
                                          UMI_TEST_PLATFORM_OPERATION_REPEAT);
    status = umi_test_rerun_plan_selection(plan, &out_operation->selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (out_operation->selection.count == 0U) return UMI_STATUS_NOT_FOUND;
    out_operation->repeat_count = policy->repeat_count;
    out_operation->stop_on_failure = policy->stop_on_failure;
    out_operation->generation = plan->revision;
    return UMI_STATUS_OK;
}

/*
 * Find test rerun plan while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_test_rerun_plan_at(
    const UmiTestRerunPlan *plan,
    size_t position,
    UmiTestRerunCandidate *out_candidate)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_candidate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= plan->count) return UMI_STATUS_NOT_FOUND;
    *out_candidate = plan->candidates[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the test rerun plan snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_rerun_plan_snapshot(
    const UmiTestRerunPlan *plan,
    UmiTestRerunPlanSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TEST_RERUN_POLICY_API_VERSION;
    out_snapshot->candidate_count = plan->count;
    out_snapshot->item_revision = plan->item_revision;
    out_snapshot->result_revision = plan->result_revision;
    out_snapshot->revision = plan->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->count; ++index) {
        const uint32_t reasons = plan->candidates[index].reason_mask;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (plan->candidates[index].selected) ++out_snapshot->selected_count;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FAILED) != 0U) {
            ++out_snapshot->failed_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_TIMED_OUT) != 0U) {
            ++out_snapshot->timed_out_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_FLAKY) != 0U) {
            ++out_snapshot->flaky_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_SLOW) != 0U) {
            ++out_snapshot->slow_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((reasons & (uint32_t)UMI_TEST_RERUN_REASON_NOT_RUN) != 0U) {
            ++out_snapshot->not_run_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by test rerun plan without changing their
 * state.
 */
size_t umi_test_rerun_plan_count(const UmiTestRerunPlan *plan)
{
    return plan != NULL ? plan->count : 0U;
}
