#include "umicom/test/workbench/test_workspace_profile.h"
#include <assert.h>
int main(void){UmiTestWorkspaceProfile m;assert(umi_test_workspace_profile_init(&m,"test_workspace_profile","Test Workspace Profile")==UMI_STATUS_OK);assert(umi_test_workspace_profile_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_workspace_profile_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_workspace_profile_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_workspace_profile_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
