/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_run_result.c
 *
 * PURPOSE:
 *   Implement the test test run result behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_run_result.h"
#include <assert.h>
int main(void){UmiTestRunResult m;assert(umi_test_run_result_init(&m,"test_run_result","Test Run Result")==UMI_STATUS_OK);assert(umi_test_run_result_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_run_result_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_run_result_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_run_result_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
