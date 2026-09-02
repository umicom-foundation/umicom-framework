/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_workspace_profile.h
 *
 * PURPOSE:
 *   Model test workspace profile state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_WORKSPACE_PROFILE_H
#define UMICOM_TEST_WORKBENCH_TEST_WORKSPACE_PROFILE_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test workspace profile data shared with callers of this public contract.
 */
typedef struct UmiTestWorkspaceProfile {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestWorkspaceProfile;
/**
 * Initialise test workspace profile from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_workspace_profile_init(UmiTestWorkspaceProfile *model,const char *id,const char *label);
/**
 * Exercise test workspace profile set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_workspace_profile_set_active(UmiTestWorkspaceProfile *model,bool active);
/**
 * Return the number of records represented by test workspace profile set without changing
 * their state.
 */
UmiStatus umi_test_workspace_profile_set_count(UmiTestWorkspaceProfile *model,uint32_t item_count);
/**
 * Exercise test workspace profile set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_workspace_profile_set_state(UmiTestWorkspaceProfile *model,UmiTestWorkbenchState state);
/**
 * Check that test workspace profile satisfies its contract before another service relies
 * on it.
 */
int umi_test_workspace_profile_valid(const UmiTestWorkspaceProfile *model);
#ifdef __cplusplus
}
#endif
#endif
