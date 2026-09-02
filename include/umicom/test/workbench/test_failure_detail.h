/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_failure_detail.h
 *
 * PURPOSE:
 *   Model test failure detail state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_FAILURE_DETAIL_H
#define UMICOM_TEST_WORKBENCH_TEST_FAILURE_DETAIL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test failure detail data shared with callers of this public contract.
 */
typedef struct UmiTestFailureDetail {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestFailureDetail;
/**
 * Initialise test failure detail from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_failure_detail_init(UmiTestFailureDetail *model,const char *id,const char *label);
/**
 * Exercise test failure detail set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_failure_detail_set_active(UmiTestFailureDetail *model,bool active);
/**
 * Return the number of records represented by test failure detail set without changing
 * their state.
 */
UmiStatus umi_test_failure_detail_set_count(UmiTestFailureDetail *model,uint32_t item_count);
/**
 * Exercise test failure detail set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_failure_detail_set_state(UmiTestFailureDetail *model,UmiTestWorkbenchState state);
/**
 * Check that test failure detail satisfies its contract before another service relies on
 * it.
 */
int umi_test_failure_detail_valid(const UmiTestFailureDetail *model);
#ifdef __cplusplus
}
#endif
#endif
