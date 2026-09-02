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
