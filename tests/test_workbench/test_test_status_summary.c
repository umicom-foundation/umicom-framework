#include "umicom/test/workbench/test_status_summary.h"
#include <assert.h>
int main(void){UmiTestStatusSummary m;assert(umi_test_status_summary_init(&m,"test_status_summary","Test Status Summary")==UMI_STATUS_OK);assert(umi_test_status_summary_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_status_summary_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_status_summary_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_status_summary_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
