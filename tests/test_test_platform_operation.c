/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_operation.c
 *
 * PURPOSE:
 *   Verify selected, repeated and cooperatively stopped test operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/test_platform/test_platform.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestPlatformOperationPlan plan;
    UmiTestPlatformOperationController controller;
    umi_test_platform_operation_plan_init(
        &plan, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    assert(umi_test_platform_operation_plan_add(&plan, "alpha") ==
           UMI_STATUS_OK);
    assert(umi_test_platform_operation_plan_add(&plan, "beta") ==
           UMI_STATUS_OK);
    assert(umi_test_platform_operation_plan_add(&plan, "alpha") ==
           UMI_STATUS_ALREADY_EXISTS);
    plan.kind = UMI_TEST_PLATFORM_OPERATION_REPEAT;
    plan.repeat_count = 3U;
    assert(umi_test_platform_operation_execution_count(&plan) == 6U);

    umi_test_platform_operation_controller_init(&controller);
    assert(umi_test_platform_operation_begin(&controller, &plan) ==
           UMI_STATUS_OK);
    assert(controller.running == 1);
    assert(controller.planned_execution_count == 6U);
    umi_test_platform_operation_mark_completed(&controller);
    assert(controller.completed_execution_count == 1U);
    assert(umi_test_platform_operation_request_stop(&controller) ==
           UMI_STATUS_OK);
    assert(umi_test_platform_operation_should_stop(&controller));
    umi_test_platform_operation_finish(&controller);
    assert(controller.running == 0);
    return 0;
}
