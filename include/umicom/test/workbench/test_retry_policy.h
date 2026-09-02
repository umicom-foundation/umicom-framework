/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_retry_policy.h
 *
 * PURPOSE:
 *   Model test retry policy state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RETRY_POLICY_H
#define UMICOM_TEST_WORKBENCH_TEST_RETRY_POLICY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test retry policy data shared with callers of this public contract.
 */
typedef struct UmiTestRetryPolicy {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRetryPolicy;
/**
 * Initialise test retry policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_retry_policy_init(UmiTestRetryPolicy *model,const char *id,const char *label);
/**
 * Exercise test retry policy set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_retry_policy_set_active(UmiTestRetryPolicy *model,bool active);
/**
 * Return the number of records represented by test retry policy set without changing their
 * state.
 */
UmiStatus umi_test_retry_policy_set_count(UmiTestRetryPolicy *model,uint32_t item_count);
/**
 * Exercise test retry policy set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_retry_policy_set_state(UmiTestRetryPolicy *model,UmiTestWorkbenchState state);
/**
 * Check that test retry policy satisfies its contract before another service relies on it.
 */
int umi_test_retry_policy_valid(const UmiTestRetryPolicy *model);
#ifdef __cplusplus
}
#endif
#endif
