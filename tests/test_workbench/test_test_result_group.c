#include "umicom/test/workbench/test_result_group.h"
#include <assert.h>
int main(void){UmiTestResultGroup m;assert(umi_test_result_group_init(&m,"test_result_group","Test Result Group")==UMI_STATUS_OK);assert(umi_test_result_group_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_result_group_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_result_group_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_result_group_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
