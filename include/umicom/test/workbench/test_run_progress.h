/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_progress.h
 *
 * PURPOSE:
 *   Model test run progress state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_PROGRESS_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_PROGRESS_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test run progress data shared with callers of this public contract.
 */
typedef struct UmiTestRunProgress {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunProgress;
/**
 * Initialise test run progress from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_run_progress_init(UmiTestRunProgress *model,const char *id,const char *label);
/**
 * Exercise test run progress set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_progress_set_active(UmiTestRunProgress *model,bool active);
/**
 * Return the number of records represented by test run progress set without changing their
 * state.
 */
UmiStatus umi_test_run_progress_set_count(UmiTestRunProgress *model,uint32_t item_count);
/**
 * Exercise test run progress set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_progress_set_state(UmiTestRunProgress *model,UmiTestWorkbenchState state);
/**
 * Check that test run progress satisfies its contract before another service relies on it.
 */
int umi_test_run_progress_valid(const UmiTestRunProgress *model);
#ifdef __cplusplus
}
#endif
#endif
