/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_diagnostic_filter_model.c
 *
 * PURPOSE:
 *   Implement the test diagnostic filter model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/diagnostic_filter_model.h"
#include <assert.h>
int main(void){UmiDiagnosticFilterModel m;assert(umi_diagnostic_filter_model_init(&m,"diagnostic_filter_model","Diagnostic Filter Model")==UMI_STATUS_OK);assert(umi_diagnostic_filter_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_diagnostic_filter_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_diagnostic_filter_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_diagnostic_filter_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
