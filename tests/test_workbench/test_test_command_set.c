#include "umicom/test/workbench/test_command_set.h"
#include <assert.h>
int main(void){UmiTestCommandSet m;assert(umi_test_command_set_init(&m,"test_command_set","Test Command Set")==UMI_STATUS_OK);assert(umi_test_command_set_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_command_set_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_command_set_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_command_set_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
