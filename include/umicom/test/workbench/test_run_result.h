/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_result.h
 *
 * PURPOSE:
 *   Model test run result state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_RESULT_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_RESULT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test run result data shared with callers of this public contract.
 */
typedef struct UmiTestRunResult {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunResult;
/**
 * Initialise test run result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_run_result_init(UmiTestRunResult *model,const char *id,const char *label);
/**
 * Exercise test run result set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_result_set_active(UmiTestRunResult *model,bool active);
/**
 * Return the number of records represented by test run result set without changing their
 * state.
 */
UmiStatus umi_test_run_result_set_count(UmiTestRunResult *model,uint32_t item_count);
/**
 * Exercise test run result set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_result_set_state(UmiTestRunResult *model,UmiTestWorkbenchState state);
/**
 * Check that test run result satisfies its contract before another service relies on it.
 */
int umi_test_run_result_valid(const UmiTestRunResult *model);
#ifdef __cplusplus
}
#endif
#endif
