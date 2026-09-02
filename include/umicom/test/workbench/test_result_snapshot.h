/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_snapshot.h
 *
 * PURPOSE:
 *   Model test result snapshot state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test result snapshot data shared with callers of this public contract.
 */
typedef struct UmiTestResultSnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultSnapshot;
/**
 * Initialise test result snapshot from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_result_snapshot_init(UmiTestResultSnapshot *model,const char *id,const char *label);
/**
 * Exercise test result snapshot set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_result_snapshot_set_active(UmiTestResultSnapshot *model,bool active);
/**
 * Return the number of records represented by test result snapshot set without changing
 * their state.
 */
UmiStatus umi_test_result_snapshot_set_count(UmiTestResultSnapshot *model,uint32_t item_count);
/**
 * Exercise test result snapshot set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_result_snapshot_set_state(UmiTestResultSnapshot *model,UmiTestWorkbenchState state);
/**
 * Check that test result snapshot satisfies its contract before another service relies on
 * it.
 */
int umi_test_result_snapshot_valid(const UmiTestResultSnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
