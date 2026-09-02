/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_parallelism_model.c
 *
 * PURPOSE:
 *   Implement the test test parallelism model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_parallelism_model.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestParallelismModel m;assert(umi_test_parallelism_model_init(&m,"test_parallelism_model","Test Parallelism Model")==UMI_STATUS_OK);assert(umi_test_parallelism_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_parallelism_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_parallelism_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_parallelism_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
