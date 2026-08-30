/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_group_model.c
 *
 * PURPOSE:
 *   Implement the test test group model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_group_model.h"
#include <assert.h>
int main(void){UmiTestGroupModel m;assert(umi_test_group_model_init(&m,"test_group_model","Test Group Model")==UMI_STATUS_OK);assert(umi_test_group_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_group_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_group_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_group_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
