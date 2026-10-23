/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/operation.c
 *
 * PURPOSE:
 *   Implement test operation planning, repetition and cooperative stopping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/operation.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/test_platform/history.h"

/* Shared validation bounds every identifier read before execution or plan edits.
 * Empty plans are valid while being assembled; begin separately requires work. */
UmiStatus umi_test_platform_operation_plan_validate(
    const UmiTestPlatformOperationPlan *plan)
{
    if (plan == NULL ||
        plan->kind < UMI_TEST_PLATFORM_OPERATION_RUN_ALL ||
        plan->kind > UMI_TEST_PLATFORM_OPERATION_REPEAT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->selection.count > UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (size_t index = 0U; index < plan->selection.count; ++index) {
        const char *id = plan->selection.item_ids[index];
        if (id[0] == '\0' ||
            memchr(id, '\0', sizeof(plan->selection.item_ids[index])) == NULL) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    const uint32_t repeats = plan->repeat_count == 0U ? 1U : plan->repeat_count;
    if (plan->selection.count > SIZE_MAX / repeats) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}


/*
 * Initialise test platform operation plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_platform_operation_plan_init(
    UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformOperationKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->kind = kind;
    plan->repeat_count = 1U;
    plan->generation = 1U;
}

/*
 * Provide the test platform operation plan all operation used by this module and its
 * client applications.
 */
/* Candidate selection publication keeps the existing plan unchanged if discovery exceeds selection capacity or supplies malformed filter text. The earlier implementation remains below for engineering review. */
#if 0
UmiStatus umi_test_platform_operation_plan_all(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter)
{
    UmiTestPlatformFilter default_filter;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || items == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL) {
        umi_test_platform_filter_init(&default_filter);
        filter = &default_filter;
    }
    plan->kind = UMI_TEST_PLATFORM_OPERATION_RUN_ALL;
    plan->generation += 1U;
    return umi_test_platform_filter_select(items, results, filter,
                                           &plan->selection);
}
#endif
UmiStatus umi_test_platform_operation_plan_all(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter)
{
    if (plan == NULL || items == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiTestPlatformFilter default_filter;
    if (filter == NULL) {
        umi_test_platform_filter_init(&default_filter);
        filter = &default_filter;
    }
    if (memchr(filter->text, '\0', sizeof(filter->text)) == NULL ||
        memchr(filter->suite_id, '\0', sizeof(filter->suite_id)) == NULL ||
        memchr(filter->label, '\0', sizeof(filter->label)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Discovery may exceed the bounded selection. Build a private candidate so
     * a capacity or validation failure leaves the user's prior plan intact. */
    UmiTestPlatformOperationPlan *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *plan;
    candidate->kind = UMI_TEST_PLATFORM_OPERATION_RUN_ALL;
    UmiStatus status = umi_test_platform_filter_select(
        items, results, filter, &candidate->selection);
    if (status == UMI_STATUS_OK)
        status = umi_test_platform_operation_plan_validate(candidate);
    if (status == UMI_STATUS_OK) {
        candidate->generation += 1U;
        *plan = *candidate;
    }
    free(candidate);
    return status;
}

/*
 * Provide the test platform operation plan failed operation used by this module and its
 * client applications.
 */
/* Candidate rerun planning preserves the current selection when retained history cannot produce a complete bounded plan. The earlier implementation remains below for engineering review. */
#if 0
UmiStatus umi_test_platform_operation_plan_failed(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || items == NULL || results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    plan->kind = UMI_TEST_PLATFORM_OPERATION_RERUN_FAILED;
    plan->generation += 1U;
    return umi_test_platform_history_failed_selection(items, results,
                                                      &plan->selection);
}
#endif
UmiStatus umi_test_platform_operation_plan_failed(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results)
{
    if (plan == NULL || items == NULL || results == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Rerun selection follows the same publish-on-success rule as run-all.
     * Earlier selected identities remain available when history is too large. */
    UmiTestPlatformOperationPlan *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *plan;
    candidate->kind = UMI_TEST_PLATFORM_OPERATION_RERUN_FAILED;
    UmiStatus status = umi_test_platform_history_failed_selection(
        items, results, &candidate->selection);
    if (status == UMI_STATUS_OK)
        status = umi_test_platform_operation_plan_validate(candidate);
    if (status == UMI_STATUS_OK) {
        candidate->generation += 1U;
        *plan = *candidate;
    }
    free(candidate);
    return status;
}

/*
 * Add test platform operation plan only after its inputs and available capacity have been
 * checked.
 */
/* Bounded plan validation and reserved observation generations replace unchecked selection reads and wrapping controller counters. Required execution and cooperative stop behavior remain in the active implementation. The earlier implementation remains below for engineering review. */
#if 0
UmiStatus umi_test_platform_operation_plan_add(
    UmiTestPlatformOperationPlan *plan,
    const char *item_id)
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
    for (index = 0U; index < plan->selection.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(plan->selection.item_ids[index], item_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->selection.count >= UMI_TEST_PLATFORM_SELECTION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)strncpy(plan->selection.item_ids[plan->selection.count], item_id,
                  sizeof(plan->selection.item_ids[0]) - 1U);
    plan->selection.count += 1U;
    plan->kind = UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED;
    plan->generation += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_platform_operation_plan_add(
    UmiTestPlatformOperationPlan *plan,
    const char *item_id)
{
    UmiStatus status = umi_test_platform_operation_plan_validate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (item_id == NULL || item_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;

    /* Reject overlong identities instead of truncating them into another test's
     * identifier. The loop stops at the caller's terminator without reading a
     * full fixed-size block from an arbitrary string pointer. */
    size_t length = 0U;
    while (length < sizeof(plan->selection.item_ids[0]) && item_id[length] != '\0')
        ++length;
    if (length == sizeof(plan->selection.item_ids[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < plan->selection.count; ++index) {
        if (strcmp(plan->selection.item_ids[index], item_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    const uint32_t repeats = plan->repeat_count == 0U ? 1U : plan->repeat_count;
    if (plan->selection.count == UMI_TEST_PLATFORM_SELECTION_CAPACITY ||
        plan->generation == UINT64_MAX ||
        plan->selection.count + 1U > SIZE_MAX / repeats)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* The caller may supply an identifier already stored in an unused slot. */
    memmove(plan->selection.item_ids[plan->selection.count], item_id, length + 1U);
    plan->selection.count += 1U;
    plan->kind = UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED;
    plan->generation += 1U;
    return UMI_STATUS_OK;
}

/* Selection assembly belongs in Framework so Studio and other frontends cannot
 * publish a valid prefix when a later requested test is missing or duplicated.
 * Build in heap storage rather than placing the bounded ID array on the stack. */
UmiStatus UmiTestPlatformOperationPlanSelected(UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items, const char *const *item_ids,
    size_t item_count)
{
    if (plan == NULL || items == NULL || item_ids == NULL || item_count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (item_count > UMI_TEST_PLATFORM_SELECTION_CAPACITY || plan->generation == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_test_platform_operation_plan_validate(plan);
    if (status != UMI_STATUS_OK) return status;
    UmiTestPlatformOperationPlan *candidate = malloc(sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_test_platform_operation_plan_init(candidate, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    candidate->repeat_count = plan->repeat_count;
    candidate->stop_on_failure = plan->stop_on_failure;
    const uint64_t revision = umi_test_platform_item_registry_revision(items);
    for (size_t index = 0U; index < item_count; ++index) {
        /* The existing bounded insertion validates each C-string before find
         * reads it, and rejects duplicate identities without truncation. */
        status = umi_test_platform_operation_plan_add(candidate, item_ids[index]);
        if (status != UMI_STATUS_OK) break;
        UmiTestPlatformItemSnapshot item;
        status = umi_test_platform_item_registry_find(items, item_ids[index], &item);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK && revision != umi_test_platform_item_registry_revision(items))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        candidate->selection.source_revision = revision;
        candidate->generation = plan->generation + 1U;
        *plan = *candidate;
    }
    free(candidate);
    return status;
}


/*
 * Return the number of records represented by test platform operation execution without
 * changing their state.
 */
size_t umi_test_platform_operation_execution_count(
    const UmiTestPlatformOperationPlan *plan)
{
    uint32_t repeats;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return 0U;
    repeats = plan->repeat_count == 0U ? 1U : plan->repeat_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->selection.count > SIZE_MAX / repeats) return SIZE_MAX;
    return plan->selection.count * (size_t)repeats;
}

/*
 * Initialise test platform operation controller from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_platform_operation_controller_init(
    UmiTestPlatformOperationController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return;
    (void)memset(controller, 0, sizeof(*controller));
    controller->generation = 1U;
}

/*
 * Provide the test platform operation begin operation used by this module and its client
 * applications.
 */
/* Bounded plan validation and reserved observation generations replace unchecked selection reads and wrapping controller counters. Required execution and cooperative stop behavior remain in the active implementation. The earlier implementation remains below for engineering review. */
#if 0
UmiStatus umi_test_platform_operation_begin(
    UmiTestPlatformOperationController *controller,
    const UmiTestPlatformOperationPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || plan == NULL || plan->selection.count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller->running) return UMI_STATUS_BUSY;
    controller->running = 1;
    controller->stop_requested = 0;
    controller->kind = plan->kind;
    controller->planned_execution_count =
        umi_test_platform_operation_execution_count(plan);
    controller->completed_execution_count = 0U;
    controller->generation += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_platform_operation_begin(
    UmiTestPlatformOperationController *controller,
    const UmiTestPlatformOperationPlan *plan)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_test_platform_operation_plan_validate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (plan->selection.count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->running) return UMI_STATUS_BUSY;
    const size_t planned = umi_test_platform_operation_execution_count(plan);

    /* Reserve generations for begin, one completion per planned execution,
     * one cooperative stop and finish. Repeated stop/completion notifications
     * are idempotent, so normal execution cannot wrap the observation token. */
    if ((uint64_t)planned > UINT64_MAX - 3U ||
        controller->generation > UINT64_MAX - (uint64_t)planned - 3U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    controller->running = 1;
    controller->stop_requested = 0;
    controller->kind = plan->kind;
    controller->planned_execution_count = planned;
    controller->completed_execution_count = 0U;
    controller->generation += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test platform operation request stop operation used by this module and its
 * client applications.
 */
/* Bounded plan validation and reserved observation generations replace unchecked selection reads and wrapping controller counters. Required execution and cooperative stop behavior remain in the active implementation. The earlier implementation remains below for engineering review. */
#if 0
UmiStatus umi_test_platform_operation_request_stop(
    UmiTestPlatformOperationController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!controller->running) return UMI_STATUS_INVALID_STATE;
    controller->stop_requested = 1;
    controller->generation += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_platform_operation_request_stop(
    UmiTestPlatformOperationController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!controller->running) return UMI_STATUS_INVALID_STATE;
    if (controller->stop_requested) return UMI_STATUS_OK;
    if (controller->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    controller->stop_requested = 1;
    controller->generation += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test platform operation should stop operation used by this module and its
 * client applications.
 */
int umi_test_platform_operation_should_stop(
    const UmiTestPlatformOperationController *controller)
{
    return controller != NULL && controller->stop_requested;
}

/*
 * Provide the test platform operation mark completed operation used by this module and its
 * client applications.
 */
/* Bounded plan validation and reserved observation generations replace unchecked selection reads and wrapping controller counters. Required execution and cooperative stop behavior remain in the active implementation. The earlier implementation remains below for engineering review. */
#if 0
void umi_test_platform_operation_mark_completed(
    UmiTestPlatformOperationController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || !controller->running) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->completed_execution_count <
        controller->planned_execution_count) {
        controller->completed_execution_count += 1U;
    }
    controller->generation += 1U;
}
#endif
void umi_test_platform_operation_mark_completed(
    UmiTestPlatformOperationController *controller)
{
    /* Begin reserves the observation budget. Ignore duplicate completions
     * after the planned count and malformed exhausted external state. */
    if (controller == NULL || !controller->running ||
        controller->completed_execution_count >= controller->planned_execution_count ||
        controller->generation == UINT64_MAX) return;
    controller->completed_execution_count += 1U;
    controller->generation += 1U;
}

/*
 * Provide the test platform operation finish operation used by this module and its client
 * applications.
 */
/* Bounded plan validation and reserved observation generations replace unchecked selection reads and wrapping controller counters. Required execution and cooperative stop behavior remain in the active implementation. The earlier implementation remains below for engineering review. */
#if 0
void umi_test_platform_operation_finish(
    UmiTestPlatformOperationController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return;
    controller->running = 0;
    controller->generation += 1U;
}
#endif
void umi_test_platform_operation_finish(
    UmiTestPlatformOperationController *controller)
{
    if (controller == NULL || !controller->running) return;
    /* Always retire running state. Begin reserves a finish generation; the
     * saturation guard also avoids wrap if a caller edited its public record. */
    controller->running = 0;
    if (controller->generation != UINT64_MAX) controller->generation += 1U;
}
