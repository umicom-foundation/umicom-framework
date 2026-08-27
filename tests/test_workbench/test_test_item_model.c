#include "umicom/test/workbench/test_item_model.h"
#include <assert.h>
int main(void){UmiTestItemModel m;assert(umi_test_item_model_init(&m,"test_item_model","Test Item Model")==UMI_STATUS_OK);assert(umi_test_item_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_item_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_item_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_item_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
