/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_quality_workbench_service.c
 *
 * PURPOSE:
 *   Implement the test test quality workbench service behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_quality_workbench_service.h"
#include <assert.h>
int main(void){UmiTestQualityWorkbenchService m;assert(umi_test_quality_workbench_service_init(&m,"test_quality_workbench_service","Test Quality Workbench Service")==UMI_STATUS_OK);assert(umi_test_quality_workbench_service_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_quality_workbench_service_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_quality_workbench_service_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_quality_workbench_service_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
