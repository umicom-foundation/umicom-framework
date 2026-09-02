/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_profile.h
 *
 * PURPOSE:
 *   Model test run profile state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_PROFILE_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_PROFILE_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test run profile data shared with callers of this public contract.
 */
typedef struct UmiTestRunProfile {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunProfile;
/**
 * Initialise test run profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_run_profile_init(UmiTestRunProfile *model,const char *id,const char *label);
/**
 * Exercise test run profile set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_profile_set_active(UmiTestRunProfile *model,bool active);
/**
 * Return the number of records represented by test run profile set without changing their
 * state.
 */
UmiStatus umi_test_run_profile_set_count(UmiTestRunProfile *model,uint32_t item_count);
/**
 * Exercise test run profile set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_profile_set_state(UmiTestRunProfile *model,UmiTestWorkbenchState state);
/**
 * Check that test run profile satisfies its contract before another service relies on it.
 */
int umi_test_run_profile_valid(const UmiTestRunProfile *model);
#ifdef __cplusplus
}
#endif
#endif
