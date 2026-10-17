/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_execution_boundaries.c
 *
 * PURPOSE:
 *   Verify execution ownership, bounded plans and truthful retained evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/test_platform.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Keep setup and verification active under NDEBUG and release every fixture
 * owner on failure. Each named case is an independent native CTest invocation. */
#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #expression); \
        goto done; \
    } \
} while (0)

typedef struct ExecuteProbe {
    const char *mode;
    UmiTestPlatformOperationPlan *caller_plan;
    UmiTestPlatformOperationController *controller;
    unsigned calls;
    uint32_t last_attempt;
} ExecuteProbe;

static UmiStatus ExecuteProbeCall(const UmiTestPlatformItemSnapshot *item,
    uint32_t attempt, void *context, UmiTestPlatformResultSnapshot *result)
{
    ExecuteProbe *probe = context;
    probe->calls += 1U;
    probe->last_attempt = attempt;
    result->outcome = UMI_TEST_PLATFORM_OUTCOME_PASSED;
    result->duration_ms = 1.25;
    (void)snprintf(result->id, sizeof(result->id), "checked.%u.%.100s",
        (unsigned)attempt, item->id);
    /* A host edit must not alter the selection currently owned by execution. */
    if (strcmp(probe->mode, "mutated-plan") == 0) {
        probe->caller_plan->selection.count = UMI_TEST_PLATFORM_SELECTION_CAPACITY + 1U;
        probe->caller_plan->repeat_count = 0U;
    } else if (strcmp(probe->mode, "maximum-repeat-stop") == 0) {
        return umi_test_platform_operation_request_stop(probe->controller);
    } else if (strcmp(probe->mode, "negative-duration") == 0) {
        result->duration_ms = -1.0;
    } else if (strcmp(probe->mode, "nan-duration") == 0) {
        result->duration_ms = NAN;
    } else if (strcmp(probe->mode, "infinite-duration") == 0) {
        result->duration_ms = INFINITY;
    } else if (strcmp(probe->mode, "range-duration") == 0) {
        result->duration_ms = 0x1p64;
    } else if (strcmp(probe->mode, "sum-duration") == 0) {
        result->duration_ms = 0x1p63;
    } else if (strcmp(probe->mode, "unknown-outcome") == 0) {
        result->outcome = 99;
    } else if (strcmp(probe->mode, "unfinished-outcome") == 0) {
        result->outcome = UMI_TEST_PLATFORM_OUTCOME_NOT_RUN;
    } else if (strcmp(probe->mode, "wrong-item") == 0) {
        (void)strcpy(result->item_id, "another-test");
    } else if (strcmp(probe->mode, "unterminated-result") == 0) {
        memset(result->message, 'x', sizeof(result->message));
    } else if (strcmp(probe->mode, "provider-error") == 0) {
        return UMI_STATUS_IO_ERROR;
    } else if (strcmp(probe->mode, "provider-timeout") == 0) {
        return UMI_STATUS_TIMEOUT;
    } else if (strcmp(probe->mode, "provider-cancelled") == 0) {
        return UMI_STATUS_CANCELLED;
    } else if (strcmp(probe->mode, "timeout-stop") == 0) {
        result->outcome = UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
    } else if (strcmp(probe->mode, "explicit-id-conflict") == 0 ||
               strcmp(probe->mode, "explicit-id-update") == 0) {
        (void)strcpy(result->id, "owned-result");
    } else if (strcmp(probe->mode, "generated-id-collision") == 0) {
        result->id[0] = '\0';
    }
    return UMI_STATUS_OK;
}

static int VerifyPlan(const char *mode)
{
    UmiTestPlatformOperationPlan *plan = malloc(sizeof(*plan));
    UmiTestPlatformOperationPlan *before = malloc(sizeof(*before));
    UmiTestPlatformOperationController controller, saved_controller;
    int result = EXIT_FAILURE;
    CHECK(plan != NULL && before != NULL);
    umi_test_platform_operation_plan_init(plan, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    umi_test_platform_operation_controller_init(&controller);
    CHECK(umi_test_platform_operation_plan_validate(plan) == UMI_STATUS_OK);
    CHECK(umi_test_platform_operation_plan_add(plan, "alpha") == UMI_STATUS_OK);

    if (strcmp(mode, "oversized-selection") == 0) {
        plan->selection.count = UMI_TEST_PLATFORM_SELECTION_CAPACITY + 1U;
        *before = *plan;
        CHECK(umi_test_platform_operation_plan_validate(plan) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_test_platform_operation_plan_add(plan, "beta") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
    } else if (strcmp(mode, "unterminated-selection") == 0) {
        memset(plan->selection.item_ids[0], 'x', sizeof(plan->selection.item_ids[0]));
        *before = *plan;
        CHECK(umi_test_platform_operation_begin(&controller, plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!controller.running);
        CHECK(umi_test_platform_operation_plan_add(plan, "beta") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
    } else if (strcmp(mode, "overlong-id") == 0) {
        char id[129];
        memset(id, 'x', sizeof(id) - 1U); id[sizeof(id) - 1U] = '\0';
        *before = *plan;
        CHECK(umi_test_platform_operation_plan_add(plan, id) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
    } else if (strcmp(mode, "addition-overflow") == 0) {
        plan->repeat_count = UINT32_MAX;
        *before = *plan;
        const int fits = SIZE_MAX / (size_t)UINT32_MAX >= 2U;
        CHECK(umi_test_platform_operation_plan_add(plan, "beta") ==
            (fits ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
        if (fits) {
            CHECK(plan->selection.count == 2U);
            CHECK(umi_test_platform_operation_plan_validate(plan) == UMI_STATUS_OK);
        } else {
            CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
        }
    } else if (strcmp(mode, "invalid-kind") == 0) {
        plan->kind = (UmiTestPlatformOperationKind)99;
        saved_controller = controller;
        CHECK(umi_test_platform_operation_begin(&controller, plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&controller, &saved_controller, sizeof(controller)) == 0);
    } else if (strcmp(mode, "plan-generation") == 0) {
        plan->generation = UINT64_MAX;
        *before = *plan;
        CHECK(umi_test_platform_operation_plan_add(plan, "beta") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
    } else if (strcmp(mode, "controller-generation") == 0) {
        controller.generation = UINT64_MAX - 3U;
        saved_controller = controller;
        CHECK(umi_test_platform_operation_begin(&controller, plan) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&controller, &saved_controller, sizeof(controller)) == 0);
    } else if (strcmp(mode, "controller-last-budget") == 0) {
        controller.generation = UINT64_MAX - 4U;
        CHECK(umi_test_platform_operation_begin(&controller, plan) == UMI_STATUS_OK);
        umi_test_platform_operation_mark_completed(&controller);
        const uint64_t completed = controller.generation;
        umi_test_platform_operation_mark_completed(&controller);
        CHECK(controller.generation == completed);
        CHECK(umi_test_platform_operation_request_stop(&controller) == UMI_STATUS_OK);
        const uint64_t stopped = controller.generation;
        CHECK(umi_test_platform_operation_request_stop(&controller) == UMI_STATUS_OK);
        CHECK(controller.generation == stopped);
        umi_test_platform_operation_finish(&controller);
        CHECK(!controller.running && controller.generation == UINT64_MAX);
        umi_test_platform_operation_finish(&controller);
        CHECK(controller.generation == UINT64_MAX);
    } else {
        goto done;
    }
    result = EXIT_SUCCESS;
done:
    free(before);
    free(plan);
    return result;
}

static int VerifyExecution(const char *mode)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformOperationPlan *plan = malloc(sizeof(*plan));
    UmiTestPlatformOperationController controller;
    UmiTestPlatformExecutionSummary summary;
    ExecuteProbe probe = {mode, plan, &controller, 0U, 0U};
    UmiStatus expected = UMI_STATUS_OK;
    unsigned expected_calls = 2U;
    size_t expected_records = 2U;
    int result = EXIT_FAILURE;
    CHECK(plan != NULL);
    CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    umi_test_platform_operation_plan_init(plan, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    umi_test_platform_operation_controller_init(&controller);
    const char *ids[] = {"alpha", "beta"};
    for (size_t index = 0U; index < 2U; ++index) {
        UmiTestPlatformItemSnapshot item = {0};
        (void)snprintf(item.id, sizeof(item.id), "%s", ids[index]);
        (void)snprintf(item.name, sizeof(item.name), "%s", ids[index]);
        (void)strcpy(item.kind, "test");
        item.enabled = 1;
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
        CHECK(umi_test_platform_operation_plan_add(plan, ids[index]) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "maximum-repeat-stop") == 0) {
        plan->selection.count = 1U;
        plan->repeat_count = UINT32_MAX;
        expected_calls = 1U; expected_records = 1U;
    } else if (strcmp(mode, "zero-repeat") == 0) {
        plan->repeat_count = 0U;
    } else if (strcmp(mode, "missing-item") == 0) {
        (void)strcpy(plan->selection.item_ids[1], "missing");
        expected = UMI_STATUS_NOT_FOUND; expected_calls = 0U; expected_records = 0U;
    } else if (strcmp(mode, "bad-plan-count") == 0) {
        plan->selection.count = UMI_TEST_PLATFORM_SELECTION_CAPACITY + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; expected_calls = 0U; expected_records = 0U;
    } else if (strcmp(mode, "sum-duration") == 0) {
        expected = UMI_STATUS_CAPACITY_EXCEEDED; expected_records = 1U;
    } else if (strcmp(mode, "provider-error") == 0 ||
               strcmp(mode, "provider-timeout") == 0 ||
               strcmp(mode, "provider-cancelled") == 0 ||
               strcmp(mode, "timeout-stop") == 0) {
        plan->stop_on_failure = strcmp(mode, "provider-cancelled") != 0;
        expected_calls = 1U; expected_records = 1U;
        if (strcmp(mode, "provider-error") == 0) expected = UMI_STATUS_IO_ERROR;
        if (strcmp(mode, "provider-timeout") == 0) expected = UMI_STATUS_TIMEOUT;
        if (strcmp(mode, "provider-cancelled") == 0) expected = UMI_STATUS_CANCELLED;
    } else if (strcmp(mode, "explicit-id-conflict") == 0 ||
               strcmp(mode, "explicit-id-update") == 0) {
        UmiTestPlatformResultSnapshot record = {0};
        (void)strcpy(record.id, "owned-result");
        const int own_result = strcmp(mode, "explicit-id-update") == 0;
        (void)strcpy(record.item_id, own_result ? "alpha" : "beta");
        record.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
        CHECK(umi_test_platform_result_registry_upsert(results, &record) == UMI_STATUS_OK);
        plan->selection.count = 1U;
        expected = own_result ? UMI_STATUS_OK : UMI_STATUS_ALREADY_EXISTS;
        expected_calls = 1U; expected_records = 1U;
    } else if (strcmp(mode, "generated-id-collision") == 0 ||
               strcmp(mode, "full-results") == 0) {
        const size_t count = strcmp(mode, "full-results") == 0
            ? UMI_TEST_PLATFORM_RESULT_CAPACITY : 1U;
        for (size_t index = 0U; index < count; ++index) {
            UmiTestPlatformResultSnapshot record = {0};
            /* Superseded: for generated-id-collision only, insertion starts
             * at revision 1 and the pre-existing upsert advances it to 2,
             * so the executed result ID is result.3, not result.2. Keep the
             * original seed for comparison and retain the full-results case. */
#if 0
            (void)snprintf(record.id, sizeof(record.id), "result.%zu", index + 2U);
#endif
            const size_t first_id = strcmp(mode, "generated-id-collision") == 0 ? 3U : 2U;
            (void)snprintf(record.id, sizeof(record.id), "result.%zu", index + first_id);
            (void)strcpy(record.item_id, "alpha");
            record.outcome = UMI_TEST_PLATFORM_OUTCOME_PASSED;
            CHECK(umi_test_platform_result_registry_upsert(results, &record) == UMI_STATUS_OK);
        }
        expected = count == 1U ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_CAPACITY_EXCEEDED;
        expected_calls = 1U; expected_records = count;
    } else if (strcmp(mode, "normal") != 0 && strcmp(mode, "mutated-plan") != 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_calls = 1U; expected_records = 0U;
    }

    const size_t initial_records = umi_test_platform_result_registry_count(results);
    const uint64_t initial_revision = umi_test_platform_result_registry_revision(results);
    memset(&summary, 0, sizeof(summary));
    CHECK(umi_test_platform_execute(items, results, plan, &controller,
        ExecuteProbeCall, &probe, &summary) == expected);
    CHECK(!controller.running);
    CHECK(probe.calls == expected_calls);
    CHECK(umi_test_platform_result_registry_count(results) == expected_records);
    CHECK(summary.executed == expected_calls);
    CHECK(umi_test_platform_result_registry_revision(results) ==
        initial_revision + (strcmp(mode, "explicit-id-update") == 0
            ? UINT64_C(1) : (uint64_t)(expected_records - initial_records)));
    if (expected_records == initial_records && strcmp(mode, "explicit-id-update") != 0) {
        CHECK(summary.passed == 0U && summary.failed == 0U && summary.skipped == 0U);
        CHECK(summary.cancelled == 0U && summary.timed_out == 0U && summary.duration_ms == 0U);
    }
    if (strcmp(mode, "normal") == 0 || strcmp(mode, "zero-repeat") == 0 ||
        strcmp(mode, "mutated-plan") == 0) {
        CHECK(summary.passed == 2U && summary.duration_ms == 2U && !summary.stopped);
        CHECK(controller.completed_execution_count == 2U);
    }
    if (strcmp(mode, "maximum-repeat-stop") == 0) {
        CHECK(summary.planned == (size_t)UINT32_MAX && summary.stopped);
        CHECK(probe.last_attempt == 1U);
    }
    if (strcmp(mode, "sum-duration") == 0) {
        CHECK(summary.duration_ms == (UINT64_C(1) << 63U) && summary.passed == 1U);
    }
    if (strcmp(mode, "explicit-id-update") == 0) {
        UmiTestPlatformResultSnapshot updated;
        CHECK(umi_test_platform_result_registry_find(results, "owned-result", &updated) == UMI_STATUS_OK);
        CHECK(updated.outcome == UMI_TEST_PLATFORM_OUTCOME_PASSED);
        CHECK(strcmp(updated.item_id, "alpha") == 0 && summary.passed == 1U);
    }
    if (strcmp(mode, "provider-error") == 0) CHECK(summary.failed == 1U && summary.stopped);
    if (strcmp(mode, "provider-timeout") == 0 || strcmp(mode, "timeout-stop") == 0)
        CHECK(summary.timed_out == 1U && summary.stopped);
    if (strcmp(mode, "provider-cancelled") == 0) CHECK(summary.cancelled == 1U && summary.stopped);
    result = EXIT_SUCCESS;
done:
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    free(plan);
    return result;
}


/* A failed selection refresh must preserve the user's complete previous plan,
 * including its generation, rather than publish a truncated list of tests. */
static int VerifySelectionPublication(const char *mode)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformOperationPlan *plan = malloc(sizeof(*plan));
    UmiTestPlatformOperationPlan *before = malloc(sizeof(*before));
    int result = EXIT_FAILURE;
    CHECK(plan != NULL && before != NULL);
    CHECK(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    umi_test_platform_operation_plan_init(plan, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    CHECK(umi_test_platform_operation_plan_add(plan, "kept-selection") == UMI_STATUS_OK);
    const int rerun = strstr(mode, "rerun") != NULL;
    const int exhausted = strstr(mode, "generation") != NULL;
    const int malformed = strcmp(mode, "selection-invalid-filter") == 0;
    const int success = strcmp(mode, "selection-success") == 0;
    if (exhausted) plan->generation = UINT64_MAX;
    *before = *plan;
    const size_t count = exhausted || malformed ? 0U
        : success ? 2U : UMI_TEST_PLATFORM_SELECTION_CAPACITY + 1U;
    for (size_t index = 0U; index < count; ++index) {
        UmiTestPlatformItemSnapshot item = {0};
        (void)snprintf(item.id, sizeof(item.id), "case-%zu", index);
        (void)snprintf(item.name, sizeof(item.name), "Case %zu", index);
        (void)strcpy(item.kind, "test");
        item.enabled = 1;
        CHECK(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
        if (rerun) {
            UmiTestPlatformResultSnapshot record = {0};
            (void)snprintf(record.id, sizeof(record.id), "result-%zu", index);
            (void)snprintf(record.item_id, sizeof(record.item_id), "%s", item.id);
            record.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
            CHECK(umi_test_platform_result_registry_upsert(results, &record) == UMI_STATUS_OK);
        }
    }
    UmiTestPlatformFilter filter;
    umi_test_platform_filter_init(&filter);
    if (malformed) memset(filter.text, 'x', sizeof(filter.text));
    const UmiStatus status = rerun
        ? umi_test_platform_operation_plan_failed(plan, items, results)
        : umi_test_platform_operation_plan_all(plan, items, results, &filter);
    if (success) {
        CHECK(status == UMI_STATUS_OK);
        CHECK(plan->kind == UMI_TEST_PLATFORM_OPERATION_RUN_ALL);
        CHECK(plan->selection.count == 2U && plan->generation == before->generation + 1U);
        CHECK(strcmp(plan->selection.item_ids[0], "case-0") == 0);
        CHECK(strcmp(plan->selection.item_ids[1], "case-1") == 0);
    } else {
        CHECK(status == (malformed ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
    }
    result = EXIT_SUCCESS;
done:
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    free(before);
    free(plan);
    return result;
}

/* Case names identify the contract boundary in CTest failure output. */
int main(int argc, char **argv)
{
    if (argc != 2) return EXIT_FAILURE;
    const char *selection_cases[] = {"selection-overflow", "selection-rerun-overflow",
        "selection-generation", "selection-rerun-generation", "selection-invalid-filter",
        "selection-success"};
    for (size_t index = 0U; index < sizeof(selection_cases) / sizeof(selection_cases[0]); ++index)
        if (strcmp(argv[1], selection_cases[index]) == 0) return VerifySelectionPublication(argv[1]);
    const char *plan_cases[] = {"oversized-selection", "unterminated-selection",
        "overlong-id", "addition-overflow", "invalid-kind", "plan-generation", "controller-generation",
        "controller-last-budget"};
    for (size_t index = 0U; index < sizeof(plan_cases) / sizeof(plan_cases[0]); ++index)
        if (strcmp(argv[1], plan_cases[index]) == 0) return VerifyPlan(argv[1]);
    const char *execution_cases[] = {"normal", "zero-repeat", "maximum-repeat-stop",
        "mutated-plan", "missing-item", "bad-plan-count", "negative-duration",
        "nan-duration", "infinite-duration", "range-duration", "sum-duration",
        "unknown-outcome", "unfinished-outcome", "wrong-item", "unterminated-result",
        "provider-error", "provider-timeout", "provider-cancelled", "timeout-stop",
        "generated-id-collision", "explicit-id-conflict", "explicit-id-update", "full-results"};
    for (size_t index = 0U; index < sizeof(execution_cases) / sizeof(execution_cases[0]); ++index)
        if (strcmp(argv[1], execution_cases[index]) == 0) return VerifyExecution(argv[1]);
    return EXIT_FAILURE;
}
