#include "umicom/test/workbench/workbench_types.h"
#include <assert.h>
int main(void){UmiTestWorkbenchEntry e;assert(umi_test_workbench_entry_init(&e,"root","All Tests")==UMI_STATUS_OK);assert(umi_test_workbench_entry_set_detail(&e,"workspace")==UMI_STATUS_OK);assert(umi_test_workbench_entry_set_state(&e,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_workbench_entry_valid(&e));assert(e.revision==3U);return 0;}
