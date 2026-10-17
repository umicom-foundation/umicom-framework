/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/execution.c
 *
 * PURPOSE:
 *   Implement provider-neutral selected/all/repeated execution and retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/execution.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Provide the record outcome operation used by this module and its client applications. */
static void record_outcome(UmiTestPlatformExecutionSummary *summary,
                           int outcome)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch ((UmiTestPlatformOutcome)outcome) {
        case UMI_TEST_PLATFORM_OUTCOME_PASSED: summary->passed += 1U; break;
        case UMI_TEST_PLATFORM_OUTCOME_FAILED: summary->failed += 1U; break;
        case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: summary->skipped += 1U; break;
        case UMI_TEST_PLATFORM_OUTCOME_CANCELLED:
            summary->cancelled += 1U;
            break;
        case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT:
            summary->timed_out += 1U;
            break;
        default: break;
    }
}

/*
 * Perform test platform through the module contract so client applications do not
 * duplicate its policy.
 */
/* Execution now owns its plan, validates provider evidence before retention, and preserves precise failures. This replaces mutable selection traversal and unchecked numeric conversions while retaining callback-based execution, repeats and cooperative stopping. The earlier implementation remains below for engineering review. */
#if 0
UmiStatus umi_test_platform_execute(
    const UmiTestPlatformItemRegistry *items,
    UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformOperationController *controller,
    UmiTestPlatformExecuteCallback callback,
    void *user_data,
    UmiTestPlatformExecutionSummary *out_summary)
{
    uint32_t repeats;
    uint32_t attempt;
    size_t selection_index;
    uint64_t sequence;
    UmiStatus first_failure = UMI_STATUS_OK;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || results == NULL || plan == NULL ||
        controller == NULL || callback == NULL || out_summary == NULL ||
        plan->selection.count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_summary, 0, sizeof(*out_summary));
    out_summary->planned = umi_test_platform_operation_execution_count(plan);
    status = umi_test_platform_operation_begin(controller, plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    repeats = plan->repeat_count == 0U ? 1U : plan->repeat_count;
    sequence = umi_test_platform_result_registry_revision(results);
    /* Visit each bounded item once so every record receives the same rule. */
    for (attempt = 1U; attempt <= repeats; ++attempt) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (selection_index = 0U;
             selection_index < plan->selection.count;
             ++selection_index) {
            UmiTestPlatformItemSnapshot item;
            UmiTestPlatformResultSnapshot result;
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_test_platform_operation_should_stop(controller)) {
                out_summary->stopped = 1;
                goto finished;
            }
            status = umi_test_platform_item_registry_find(
                items, plan->selection.item_ids[selection_index], &item);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (first_failure == UMI_STATUS_OK) first_failure = status;
                continue;
            }
            (void)memset(&result, 0, sizeof(result));
            result.struct_size = (uint32_t)sizeof(result);
            result.api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
            result.sequence = ++sequence;
            (void)strncpy(result.item_id, item.id,
                          sizeof(result.item_id) - 1U);
            status = callback(&item, attempt, user_data, &result);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (result.id[0] == '\0') {
                int written = snprintf(result.id, sizeof(result.id),
                                       "result.%llu",
                                       (unsigned long long)result.sequence);
                /* Apply this branch only when its contract condition is satisfied. */
                if (written < 0 ||
                    (size_t)written >= sizeof(result.id)) {
                    status = UMI_STATUS_CAPACITY_EXCEEDED;
                }
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (result.item_id[0] == '\0') {
                (void)strncpy(result.item_id, item.id,
                              sizeof(result.item_id) - 1U);
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_test_platform_result_registry_upsert(results, &result) !=
                    UMI_STATUS_OK && first_failure == UMI_STATUS_OK) {
                first_failure = UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_summary->executed += 1U;
            out_summary->duration_ms += (uint64_t)result.duration_ms;
            record_outcome(out_summary, result.outcome);
            umi_test_platform_operation_mark_completed(controller);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK && first_failure == UMI_STATUS_OK) {
                first_failure = status;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (plan->stop_on_failure &&
                result.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED) {
                (void)umi_test_platform_operation_request_stop(controller);
            }
        }
    }
finished:
    umi_test_platform_operation_finish(controller);
    return first_failure;
}
#endif
UmiStatus umi_test_platform_execute(
    const UmiTestPlatformItemRegistry *items,
    UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformOperationController *controller,
    UmiTestPlatformExecuteCallback callback,
    void *user_data,
    UmiTestPlatformExecutionSummary *out_summary)
{
    if (items == NULL || results == NULL || controller == NULL ||
        callback == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_test_platform_operation_plan_validate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (plan->selection.count == 0U) return UMI_STATUS_INVALID_ARGUMENT;

    /* The provider may invoke host callbacks. Own a stable plan rather than
     * iterating a caller-owned selection that could change during execution.
     * The bounded selection is large enough to keep off the native thread stack. */
    UmiTestPlatformOperationPlan *owned_plan = malloc(sizeof(*owned_plan));
    if (owned_plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *owned_plan = *plan;
    memset(out_summary, 0, sizeof(*out_summary));
    out_summary->planned = umi_test_platform_operation_execution_count(owned_plan);
    for (size_t index = 0U; index < owned_plan->selection.count; ++index) {
        UmiTestPlatformItemSnapshot item;
        status = umi_test_platform_item_registry_find(
            items, owned_plan->selection.item_ids[index], &item);
        if (status != UMI_STATUS_OK) {
            free(owned_plan);
            return status;
        }
    }
    if (umi_test_platform_result_registry_revision(results) == UINT64_MAX) {
        free(owned_plan);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_test_platform_operation_begin(controller, owned_plan);
    if (status != UMI_STATUS_OK) {
        free(owned_plan);
        return status;
    }

    UmiStatus first_failure = UMI_STATUS_OK;
    const uint32_t repeats = owned_plan->repeat_count == 0U ? 1U : owned_plan->repeat_count;
    uint64_t sequence = umi_test_platform_result_registry_revision(results);
    /* A zero-based counter terminates even when repeat_count is UINT32_MAX.
     * Providers still receive the established one-based attempt number. */
    for (uint32_t attempt = 0U; attempt < repeats; ++attempt) {
        for (size_t index = 0U; index < owned_plan->selection.count; ++index) {
            UmiTestPlatformItemSnapshot item;
            UmiTestPlatformResultSnapshot result;
            if (umi_test_platform_operation_should_stop(controller)) {
                out_summary->stopped = 1;
                goto finished;
            }
            status = umi_test_platform_item_registry_find(
                items, owned_plan->selection.item_ids[index], &item);
            if (status != UMI_STATUS_OK) {
                if (first_failure == UMI_STATUS_OK) first_failure = status;
                goto finished;
            }
            const uint64_t revision = umi_test_platform_result_registry_revision(results);
            if (revision > sequence) sequence = revision;
            if (sequence == UINT64_MAX) {
                if (first_failure == UMI_STATUS_OK) first_failure = UMI_STATUS_CAPACITY_EXCEEDED;
                goto finished;
            }
            memset(&result, 0, sizeof(result));
            result.struct_size = (uint32_t)sizeof(result);
            result.api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
            result.sequence = ++sequence;
            memcpy(result.item_id, item.id, strlen(item.id) + 1U);
            status = callback(&item, attempt + 1U, user_data, &result);
            out_summary->executed += 1U;
            umi_test_platform_operation_mark_completed(controller);
            if (status != UMI_STATUS_OK && first_failure == UMI_STATUS_OK)
                first_failure = status;

            /* Provider failure cannot publish a successful execution. Keep its
             * message and failure details, but make the retained outcome agree
             * with cancellation, timeout or failure of the provider operation. */
            if (status != UMI_STATUS_OK) {
                result.outcome = status == UMI_STATUS_CANCELLED
                    ? UMI_TEST_PLATFORM_OUTCOME_CANCELLED
                    : status == UMI_STATUS_TIMEOUT ? UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT
                    : UMI_TEST_PLATFORM_OUTCOME_FAILED;
            }
            result.sequence = sequence;
            if (result.id[0] == '\0') {
                (void)snprintf(result.id, sizeof(result.id), "result.%llu",
                    (unsigned long long)sequence);
                UmiTestPlatformResultSnapshot existing;
                UmiStatus found = umi_test_platform_result_registry_find(results, result.id, &existing);
                if (found != UMI_STATUS_NOT_FOUND) {
                    if (first_failure == UMI_STATUS_OK)
                        first_failure = found == UMI_STATUS_OK ? UMI_STATUS_ALREADY_EXISTS : found;
                    goto finished;
                }
            }
            if (result.item_id[0] == '\0')
                memcpy(result.item_id, item.id, strlen(item.id) + 1U);

            /* Validate before string comparison, numeric conversion or retention.
             * Invalid provider evidence stops execution with the original error;
             * it must not enter the registry as an apparent completed pass. */
            UmiStatus result_status = umi_test_platform_result_snapshot_validate(&result, NULL);
            if (result_status == UMI_STATUS_OK &&
                (strcmp(result.item_id, item.id) != 0 ||
                 result.outcome < UMI_TEST_PLATFORM_OUTCOME_PASSED ||
                 result.outcome > UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT ||
                 !isfinite(result.duration_ms) || result.duration_ms < 0.0 ||
                 result.duration_ms >= 0x1p64)) {
                result_status = UMI_STATUS_INVALID_ARGUMENT;
            }
            uint64_t duration = 0U;
            if (result_status == UMI_STATUS_OK) {
                duration = (uint64_t)result.duration_ms;
                if (duration > UINT64_MAX - out_summary->duration_ms)
                    result_status = UMI_STATUS_CAPACITY_EXCEEDED;
            }
            if (result_status == UMI_STATUS_OK) {
                /* Explicit provider ids may update their own test result, but
                 * cannot erase evidence belonging to a different selected test. */
                UmiTestPlatformResultSnapshot existing;
                const UmiStatus found = umi_test_platform_result_registry_find(results, result.id, &existing);
                if (found == UMI_STATUS_OK && strcmp(existing.item_id, item.id) != 0)
                    result_status = UMI_STATUS_ALREADY_EXISTS;
                else if (found != UMI_STATUS_OK && found != UMI_STATUS_NOT_FOUND)
                    result_status = found;
            }
            if (result_status == UMI_STATUS_OK)
                result_status = umi_test_platform_result_registry_upsert(results, &result);
            if (result_status != UMI_STATUS_OK) {
                if (first_failure == UMI_STATUS_OK) first_failure = result_status;
                goto finished;
            }
            out_summary->duration_ms += duration;
            record_outcome(out_summary, result.outcome);
            if (status == UMI_STATUS_CANCELLED ||
                (owned_plan->stop_on_failure &&
                 (status != UMI_STATUS_OK ||
                  result.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED ||
                  result.outcome == UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT))) {
                (void)umi_test_platform_operation_request_stop(controller);
            }
        }
    }
finished:
    if (umi_test_platform_operation_should_stop(controller)) out_summary->stopped = 1;
    umi_test_platform_operation_finish(controller);
    free(owned_plan);
    return first_failure;
}
