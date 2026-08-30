/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/launch_plan.h
 *
 * PURPOSE:
 *   Describe an immutable executable, arguments, environment and timeout launch plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_LAUNCH_PLAN
#define UMICOM_TEST_RUNTIME_LAUNCH_PLAN

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeLaunchPlan {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t argument_count;
    uint64_t environment_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeLaunchPlan;

void umi_test_runtime_launch_plan_init(UmiTestRuntimeLaunchPlan *value, const char *id);
UmiStatus umi_test_runtime_launch_plan_validate(const UmiTestRuntimeLaunchPlan *value);
UmiStatus umi_test_runtime_launch_plan_set_name(UmiTestRuntimeLaunchPlan *value, const char *name);
UmiStatus umi_test_runtime_launch_plan_set_detail(UmiTestRuntimeLaunchPlan *value, const char *detail);
UmiStatus umi_test_runtime_launch_plan_set_argument_count(UmiTestRuntimeLaunchPlan *value, uint64_t number);
UmiStatus umi_test_runtime_launch_plan_set_environment_count(UmiTestRuntimeLaunchPlan *value, uint64_t number);
UmiStatus umi_test_runtime_launch_plan_touch(UmiTestRuntimeLaunchPlan *value, uint64_t updated_at_ms);
bool umi_test_runtime_launch_plan_same_identity(const UmiTestRuntimeLaunchPlan *left, const UmiTestRuntimeLaunchPlan *right);

#ifdef __cplusplus
}
#endif
#endif
