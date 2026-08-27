#include "umicom/test/workbench/flaky_test_model.h"
#include <assert.h>
int main(void){UmiFlakyTestModel m;assert(umi_flaky_test_model_init(&m,"flaky_test_model","Flaky Test Model")==UMI_STATUS_OK);assert(umi_flaky_test_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_flaky_test_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_flaky_test_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_flaky_test_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
