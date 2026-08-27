#include "umicom/test/workbench/test_tree_selection.h"
#include <assert.h>
int main(void){UmiTestTreeSelection m;assert(umi_test_tree_selection_init(&m,"test_tree_selection","Test Tree Selection")==UMI_STATUS_OK);assert(umi_test_tree_selection_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_tree_selection_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_tree_selection_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_tree_selection_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
