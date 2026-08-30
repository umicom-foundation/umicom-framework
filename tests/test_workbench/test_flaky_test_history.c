/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_flaky_test_history.c
 *
 * PURPOSE:
 *   Implement the test flaky test history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/flaky_test_history.h"
#include <assert.h>
int main(void){UmiFlakyTestHistory m;assert(umi_flaky_test_history_init(&m,"flaky_test_history","Flaky Test History")==UMI_STATUS_OK);assert(umi_flaky_test_history_set_active(&m,true)==UMI_STATUS_OK);assert(umi_flaky_test_history_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_flaky_test_history_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_flaky_test_history_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
