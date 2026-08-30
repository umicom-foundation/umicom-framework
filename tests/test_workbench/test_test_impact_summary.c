/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_impact_summary.c
 *
 * PURPOSE:
 *   Implement the test test impact summary behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_impact_summary.h"
#include <assert.h>
int main(void){UmiTestImpactSummary m;assert(umi_test_impact_summary_init(&m,"test_impact_summary","Test Impact Summary")==UMI_STATUS_OK);assert(umi_test_impact_summary_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_impact_summary_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_impact_summary_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_impact_summary_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
