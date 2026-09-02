/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_status_summary.c
 *
 * PURPOSE:
 *   Implement the test test status summary behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_status_summary.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestStatusSummary m;assert(umi_test_status_summary_init(&m,"test_status_summary","Test Status Summary")==UMI_STATUS_OK);assert(umi_test_status_summary_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_status_summary_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_status_summary_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_status_summary_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
