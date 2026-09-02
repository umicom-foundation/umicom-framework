/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_debug_plan.h
 *
 * PURPOSE:
 *   Model test debug plan state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_DEBUG_PLAN_H
#define UMICOM_TEST_WORKBENCH_TEST_DEBUG_PLAN_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test debug plan data shared with callers of this public contract.
 */
typedef struct UmiTestDebugPlan {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDebugPlan;
/**
 * Initialise test debug plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_debug_plan_init(UmiTestDebugPlan *model,const char *id,const char *label);
/**
 * Exercise test debug plan set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_debug_plan_set_active(UmiTestDebugPlan *model,bool active);
/**
 * Return the number of records represented by test debug plan set without changing their
 * state.
 */
UmiStatus umi_test_debug_plan_set_count(UmiTestDebugPlan *model,uint32_t item_count);
/**
 * Exercise test debug plan set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_debug_plan_set_state(UmiTestDebugPlan *model,UmiTestWorkbenchState state);
/**
 * Check that test debug plan satisfies its contract before another service relies on it.
 */
int umi_test_debug_plan_valid(const UmiTestDebugPlan *model);
#ifdef __cplusplus
}
#endif
#endif
