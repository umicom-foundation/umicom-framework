#include "umicom/test/workbench/test_output_entry.h"
#include <assert.h>
int main(void){UmiTestOutputEntry m;assert(umi_test_output_entry_init(&m,"test_output_entry","Test Output Entry")==UMI_STATUS_OK);assert(umi_test_output_entry_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_output_entry_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_output_entry_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_output_entry_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
