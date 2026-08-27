#include "umicom/test/workbench/test_result_snapshot.h"
#include <assert.h>
int main(void){UmiTestResultSnapshot m;assert(umi_test_result_snapshot_init(&m,"test_result_snapshot","Test Result Snapshot")==UMI_STATUS_OK);assert(umi_test_result_snapshot_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_result_snapshot_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_result_snapshot_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_result_snapshot_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
