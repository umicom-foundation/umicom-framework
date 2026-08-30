/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_tree_filter.c
 *
 * PURPOSE:
 *   Implement the test test tree filter behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_tree_filter.h"
#include <assert.h>
int main(void){UmiTestTreeFilter m;assert(umi_test_tree_filter_init(&m,"test_tree_filter","Test Tree Filter")==UMI_STATUS_OK);assert(umi_test_tree_filter_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_tree_filter_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_tree_filter_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_tree_filter_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
