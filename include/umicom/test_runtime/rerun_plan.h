/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/rerun_plan.h
 *
 * PURPOSE:
 *   Plan explicit reruns for failed tests without masking original evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RERUN_PLAN
#define UMICOM_TEST_RUNTIME_RERUN_PLAN
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime rerun plan data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeRerunPlan
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t test_count;
    uint64_t attempt;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeRerunPlan;
/**
 * Initialise test runtime rerun plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_rerun_plan_init(UmiTestRuntimeRerunPlan *value,const char *id);
/**
 * Check that test runtime rerun plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_rerun_plan_validate(const UmiTestRuntimeRerunPlan *value);
/**
 * Provide the test runtime rerun plan set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_rerun_plan_set_detail(UmiTestRuntimeRerunPlan *value,const char *detail);
/**
 * Return the number of records represented by test runtime rerun plan set test without
 * changing their state.
 */
UmiStatus umi_test_runtime_rerun_plan_set_test_count(UmiTestRuntimeRerunPlan *value,uint64_t number);
/**
 * Provide the test runtime rerun plan set attempt operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_rerun_plan_set_attempt(UmiTestRuntimeRerunPlan *value,uint64_t number);
/**
 * Provide the test runtime rerun plan same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_rerun_plan_same_identity(const UmiTestRuntimeRerunPlan *left,const UmiTestRuntimeRerunPlan *right);
#ifdef __cplusplus
}
#endif
#endif
