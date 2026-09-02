/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/flaky_test_history.h
 *
 * PURPOSE:
 *   Model flaky test history state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_FLAKY_TEST_HISTORY_H
#define UMICOM_TEST_WORKBENCH_FLAKY_TEST_HISTORY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the flaky test history data shared with callers of this public contract.
 */
typedef struct UmiFlakyTestHistory {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiFlakyTestHistory;
/**
 * Initialise flaky test history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_flaky_test_history_init(UmiFlakyTestHistory *model,const char *id,const char *label);
/**
 * Exercise flaky test history set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_flaky_test_history_set_active(UmiFlakyTestHistory *model,bool active);
/**
 * Return the number of records represented by flaky test history set without changing
 * their state.
 */
UmiStatus umi_flaky_test_history_set_count(UmiFlakyTestHistory *model,uint32_t item_count);
/**
 * Exercise flaky test history set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_flaky_test_history_set_state(UmiFlakyTestHistory *model,UmiTestWorkbenchState state);
/**
 * Check that flaky test history satisfies its contract before another service relies on
 * it.
 */
int umi_flaky_test_history_valid(const UmiFlakyTestHistory *model);
#ifdef __cplusplus
}
#endif
#endif
