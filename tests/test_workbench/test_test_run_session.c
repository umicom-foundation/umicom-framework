#include "umicom/test/workbench/test_run_session.h"
#include <assert.h>
int main(void){UmiTestRunSession m;assert(umi_test_run_session_init(&m,"test_run_session","Test Run Session")==UMI_STATUS_OK);assert(umi_test_run_session_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_run_session_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_run_session_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_run_session_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
