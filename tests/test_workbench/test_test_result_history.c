/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_result_history.c
 *
 * PURPOSE:
 *   Implement the test test result history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_result_history.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestResultHistory m;assert(umi_test_result_history_init(&m,"test_result_history","Test Result History")==UMI_STATUS_OK);assert(umi_test_result_history_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_result_history_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_result_history_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_result_history_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
