/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_launch_plan.c
 *
 * PURPOSE:
 *   Verify the launch plan contract, bounded text and revision behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/launch_plan.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeLaunchPlan value;
    UmiTestRuntimeLaunchPlan same;
    uint64_t revision;
    umi_test_runtime_launch_plan_init(&value, "test-runtime.launch_plan");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_launch_plan_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_launch_plan_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_launch_plan_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_launch_plan_set_argument_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_launch_plan_set_environment_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_launch_plan_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.argument_count == 7U);
    assert(value.environment_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_launch_plan_init(&same, "test-runtime.launch_plan");
    assert(umi_test_runtime_launch_plan_same_identity(&value, &same));
    assert(umi_test_runtime_launch_plan_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
