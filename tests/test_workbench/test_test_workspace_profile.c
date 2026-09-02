/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_workspace_profile.c
 *
 * PURPOSE:
 *   Implement the test test workspace profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_workspace_profile.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestWorkspaceProfile m;assert(umi_test_workspace_profile_init(&m,"test_workspace_profile","Test Workspace Profile")==UMI_STATUS_OK);assert(umi_test_workspace_profile_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_workspace_profile_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_workspace_profile_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_workspace_profile_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
