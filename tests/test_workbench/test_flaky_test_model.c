/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_flaky_test_model.c
 *
 * PURPOSE:
 *   Implement the test flaky test model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/flaky_test_model.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiFlakyTestModel m;assert(umi_flaky_test_model_init(&m,"flaky_test_model","Flaky Test Model")==UMI_STATUS_OK);assert(umi_flaky_test_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_flaky_test_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_flaky_test_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_flaky_test_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
