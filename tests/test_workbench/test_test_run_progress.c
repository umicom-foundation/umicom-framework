/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_run_progress.c
 *
 * PURPOSE:
 *   Implement the test test run progress behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_run_progress.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestRunProgress m;assert(umi_test_run_progress_init(&m,"test_run_progress","Test Run Progress")==UMI_STATUS_OK);assert(umi_test_run_progress_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_run_progress_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_run_progress_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_run_progress_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
