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

static void record_outcome(UmiTestPlatformExecutionSummary *summary,
                           int outcome)
{
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
    if (items == NULL || results == NULL || plan == NULL ||
        controller == NULL || callback == NULL || out_summary == NULL ||
        plan->selection.count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_summary, 0, sizeof(*out_summary));
    out_summary->planned = umi_test_platform_operation_execution_count(plan);
    status = umi_test_platform_operation_begin(controller, plan);
    if (status != UMI_STATUS_OK) return status;
    repeats = plan->repeat_count == 0U ? 1U : plan->repeat_count;
    sequence = umi_test_platform_result_registry_revision(results);
    for (attempt = 1U; attempt <= repeats; ++attempt) {
        for (selection_index = 0U;
             selection_index < plan->selection.count;
             ++selection_index) {
            UmiTestPlatformItemSnapshot item;
            UmiTestPlatformResultSnapshot result;
            if (umi_test_platform_operation_should_stop(controller)) {
                out_summary->stopped = 1;
                goto finished;
            }
            status = umi_test_platform_item_registry_find(
                items, plan->selection.item_ids[selection_index], &item);
            if (status != UMI_STATUS_OK) {
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
            if (result.id[0] == '\0') {
                int written = snprintf(result.id, sizeof(result.id),
                                       "result.%llu",
                                       (unsigned long long)result.sequence);
                if (written < 0 ||
                    (size_t)written >= sizeof(result.id)) {
                    status = UMI_STATUS_CAPACITY_EXCEEDED;
                }
            }
            if (result.item_id[0] == '\0') {
                (void)strncpy(result.item_id, item.id,
                              sizeof(result.item_id) - 1U);
            }
            if (umi_test_platform_result_registry_upsert(results, &result) !=
                    UMI_STATUS_OK && first_failure == UMI_STATUS_OK) {
                first_failure = UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_summary->executed += 1U;
            out_summary->duration_ms += (uint64_t)result.duration_ms;
            record_outcome(out_summary, result.outcome);
            umi_test_platform_operation_mark_completed(controller);
            if (status != UMI_STATUS_OK && first_failure == UMI_STATUS_OK) {
                first_failure = status;
            }
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
