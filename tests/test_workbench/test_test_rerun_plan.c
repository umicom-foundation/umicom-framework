#include "umicom/test/workbench/test_rerun_plan.h"
#include <assert.h>
int main(void){UmiTestRerunPlan m;assert(umi_test_rerun_plan_init(&m,"test_rerun_plan","Test Rerun Plan")==UMI_STATUS_OK);assert(umi_test_rerun_plan_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_rerun_plan_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_rerun_plan_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_rerun_plan_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
