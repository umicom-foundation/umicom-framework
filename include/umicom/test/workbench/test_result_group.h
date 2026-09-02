/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_group.h
 *
 * PURPOSE:
 *   Model test result group state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_GROUP_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_GROUP_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test result group data shared with callers of this public contract.
 */
typedef struct UmiTestResultGroup {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultGroup;
/**
 * Initialise test result group from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_result_group_init(UmiTestResultGroup *model,const char *id,const char *label);
/**
 * Exercise test result group set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_result_group_set_active(UmiTestResultGroup *model,bool active);
/**
 * Return the number of records represented by test result group set without changing their
 * state.
 */
UmiStatus umi_test_result_group_set_count(UmiTestResultGroup *model,uint32_t item_count);
/**
 * Exercise test result group set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_result_group_set_state(UmiTestResultGroup *model,UmiTestWorkbenchState state);
/**
 * Check that test result group satisfies its contract before another service relies on it.
 */
int umi_test_result_group_valid(const UmiTestResultGroup *model);
#ifdef __cplusplus
}
#endif
#endif
