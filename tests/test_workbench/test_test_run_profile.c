/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_run_profile.c
 *
 * PURPOSE:
 *   Implement the test test run profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_run_profile.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestRunProfile m;assert(umi_test_run_profile_init(&m,"test_run_profile","Test Run Profile")==UMI_STATUS_OK);assert(umi_test_run_profile_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_run_profile_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_run_profile_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_run_profile_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
