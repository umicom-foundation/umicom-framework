/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/operation.c
 *
 * PURPOSE:
 *   Implement test operation planning, repetition and cooperative stopping.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/operation.h"

#include <string.h>

#include "umicom/test_platform/history.h"

void umi_test_platform_operation_plan_init(
    UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformOperationKind kind)
{
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->kind = kind;
    plan->repeat_count = 1U;
    plan->generation = 1U;
}

UmiStatus umi_test_platform_operation_plan_all(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter)
{
    UmiTestPlatformFilter default_filter;
    if (plan == NULL || items == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (filter == NULL) {
        umi_test_platform_filter_init(&default_filter);
        filter = &default_filter;
    }
    plan->kind = UMI_TEST_PLATFORM_OPERATION_RUN_ALL;
    plan->generation += 1U;
    return umi_test_platform_filter_select(items, results, filter,
                                           &plan->selection);
}

UmiStatus umi_test_platform_operation_plan_failed(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results)
{
    if (plan == NULL || items == NULL || results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    plan->kind = UMI_TEST_PLATFORM_OPERATION_RERUN_FAILED;
    plan->generation += 1U;
    return umi_test_platform_history_failed_selection(items, results,
                                                      &plan->selection);
}

UmiStatus umi_test_platform_operation_plan_add(
    UmiTestPlatformOperationPlan *plan,
    const char *item_id)
{
    size_t index;
    if (plan == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < plan->selection.count; ++index) {
        if (strcmp(plan->selection.item_ids[index], item_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
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

size_t umi_test_platform_operation_execution_count(
    const UmiTestPlatformOperationPlan *plan)
{
    uint32_t repeats;
    if (plan == NULL) return 0U;
    repeats = plan->repeat_count == 0U ? 1U : plan->repeat_count;
    if (plan->selection.count > SIZE_MAX / repeats) return SIZE_MAX;
    return plan->selection.count * (size_t)repeats;
}

void umi_test_platform_operation_controller_init(
    UmiTestPlatformOperationController *controller)
{
    if (controller == NULL) return;
    (void)memset(controller, 0, sizeof(*controller));
    controller->generation = 1U;
}

UmiStatus umi_test_platform_operation_begin(
    UmiTestPlatformOperationController *controller,
    const UmiTestPlatformOperationPlan *plan)
{
    if (controller == NULL || plan == NULL || plan->selection.count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
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

UmiStatus umi_test_platform_operation_request_stop(
    UmiTestPlatformOperationController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!controller->running) return UMI_STATUS_INVALID_STATE;
    controller->stop_requested = 1;
    controller->generation += 1U;
    return UMI_STATUS_OK;
}

int umi_test_platform_operation_should_stop(
    const UmiTestPlatformOperationController *controller)
{
    return controller != NULL && controller->stop_requested;
}

void umi_test_platform_operation_mark_completed(
    UmiTestPlatformOperationController *controller)
{
    if (controller == NULL || !controller->running) return;
    if (controller->completed_execution_count <
        controller->planned_execution_count) {
        controller->completed_execution_count += 1U;
    }
    controller->generation += 1U;
}

void umi_test_platform_operation_finish(
    UmiTestPlatformOperationController *controller)
{
    if (controller == NULL) return;
    controller->running = 0;
    controller->generation += 1U;
}
