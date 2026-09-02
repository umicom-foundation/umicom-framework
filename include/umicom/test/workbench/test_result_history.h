/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_history.h
 *
 * PURPOSE:
 *   Model test result history state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_HISTORY_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_HISTORY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test result history data shared with callers of this public contract.
 */
typedef struct UmiTestResultHistory {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultHistory;
/**
 * Initialise test result history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_result_history_init(UmiTestResultHistory *model,const char *id,const char *label);
/**
 * Exercise test result history set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_result_history_set_active(UmiTestResultHistory *model,bool active);
/**
 * Return the number of records represented by test result history set without changing
 * their state.
 */
UmiStatus umi_test_result_history_set_count(UmiTestResultHistory *model,uint32_t item_count);
/**
 * Exercise test result history set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_result_history_set_state(UmiTestResultHistory *model,UmiTestWorkbenchState state);
/**
 * Check that test result history satisfies its contract before another service relies on
 * it.
 */
int umi_test_result_history_valid(const UmiTestResultHistory *model);
#ifdef __cplusplus
}
#endif
#endif
