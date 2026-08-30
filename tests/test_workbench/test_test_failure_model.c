/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_failure_model.c
 *
 * PURPOSE:
 *   Implement the test test failure model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_failure_model.h"
#include <assert.h>
int main(void){UmiTestFailureModel m;assert(umi_test_failure_model_init(&m,"test_failure_model","Test Failure Model")==UMI_STATUS_OK);assert(umi_test_failure_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_failure_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_failure_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_failure_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
