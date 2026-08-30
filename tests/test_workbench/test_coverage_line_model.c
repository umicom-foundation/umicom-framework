/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_coverage_line_model.c
 *
 * PURPOSE:
 *   Implement the test coverage line model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/coverage_line_model.h"
#include <assert.h>
int main(void){UmiCoverageLineModel m;assert(umi_coverage_line_model_init(&m,"coverage_line_model","Coverage Line Model")==UMI_STATUS_OK);assert(umi_coverage_line_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_coverage_line_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_coverage_line_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_coverage_line_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
