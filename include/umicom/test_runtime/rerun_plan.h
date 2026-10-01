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
/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_test_runtime_rerun_plan_init_checked(UmiTestRuntimeRerunPlan *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
