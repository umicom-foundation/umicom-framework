/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_request.h
 *
 * PURPOSE:
 *   Model test run request state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_REQUEST_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_REQUEST_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test run request data shared with callers of this public contract.
 */
typedef struct UmiTestRunRequest {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunRequest;
/**
 * Initialise test run request from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_run_request_init(UmiTestRunRequest *model,const char *id,const char *label);
/**
 * Exercise test run request set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_request_set_active(UmiTestRunRequest *model,bool active);
/**
 * Return the number of records represented by test run request set without changing their
 * state.
 */
UmiStatus umi_test_run_request_set_count(UmiTestRunRequest *model,uint32_t item_count);
/**
 * Exercise test run request set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_request_set_state(UmiTestRunRequest *model,UmiTestWorkbenchState state);
/**
 * Check that test run request satisfies its contract before another service relies on it.
 */
int umi_test_run_request_valid(const UmiTestRunRequest *model);
#ifdef __cplusplus
}
#endif
#endif
