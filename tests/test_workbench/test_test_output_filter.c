/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_output_filter.c
 *
 * PURPOSE:
 *   Implement the test test output filter behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_output_filter.h"
#include <assert.h>
int main(void){UmiTestOutputFilter m;assert(umi_test_output_filter_init(&m,"test_output_filter","Test Output Filter")==UMI_STATUS_OK);assert(umi_test_output_filter_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_output_filter_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_output_filter_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_output_filter_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
