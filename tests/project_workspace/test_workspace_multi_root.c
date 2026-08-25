#include "umicom/project/workspace/workspace_multi_root.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceWorkspaceMultiRoot v;
    umi_project_workspace_workspace_multi_root_init(&v);
    CHECK(!umi_project_workspace_workspace_multi_root_valid(&v));
    CHECK(umi_project_workspace_workspace_multi_root_add_root(&v,"C:/code/a")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_multi_root_valid(&v));
    return 0;
}
