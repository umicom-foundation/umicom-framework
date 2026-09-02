/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_result_item.c
 *
 * PURPOSE:
 *   Implement the test test result item behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_result_item.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestResultItem m;assert(umi_test_result_item_init(&m,"test_result_item","Test Result Item")==UMI_STATUS_OK);assert(umi_test_result_item_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_result_item_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_result_item_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_result_item_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
