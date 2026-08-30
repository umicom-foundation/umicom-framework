/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_quality_budget_model.c
 *
 * PURPOSE:
 *   Implement the test quality budget model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/quality_budget_model.h"
#include <assert.h>
int main(void){UmiQualityBudgetModel m;assert(umi_quality_budget_model_init(&m,"quality_budget_model","Quality Budget Model")==UMI_STATUS_OK);assert(umi_quality_budget_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_quality_budget_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_quality_budget_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_quality_budget_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
