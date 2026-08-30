/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_impact_model.c
 *
 * PURPOSE:
 *   Implement the test test impact model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_impact_model.h"
#include <assert.h>
int main(void){UmiTestImpactModel m;assert(umi_test_impact_model_init(&m,"test_impact_model","Test Impact Model")==UMI_STATUS_OK);assert(umi_test_impact_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_impact_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_impact_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_impact_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
