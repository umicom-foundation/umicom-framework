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
typedef struct UmiTestWorkspaceProfile {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestWorkspaceProfile;
UmiStatus umi_test_workspace_profile_init(UmiTestWorkspaceProfile *model,const char *id,const char *label);
UmiStatus umi_test_workspace_profile_set_active(UmiTestWorkspaceProfile *model,bool active);
UmiStatus umi_test_workspace_profile_set_count(UmiTestWorkspaceProfile *model,uint32_t item_count);
UmiStatus umi_test_workspace_profile_set_state(UmiTestWorkspaceProfile *model,UmiTestWorkbenchState state);
int umi_test_workspace_profile_valid(const UmiTestWorkspaceProfile *model);
#ifdef __cplusplus
}
#endif
#endif
