/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_debug_plan.c
 *
 * PURPOSE:
 *   Implement the test test debug plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_debug_plan.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestDebugPlan m;assert(umi_test_debug_plan_init(&m,"test_debug_plan","Test Debug Plan")==UMI_STATUS_OK);assert(umi_test_debug_plan_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_debug_plan_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_debug_plan_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_debug_plan_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
