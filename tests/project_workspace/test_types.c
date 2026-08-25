#include "umicom/project/workspace/types.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceNamedState v,w;
    CHECK(umi_project_workspace_named_state_init(&v,"x")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_named_state_init(&w,"x")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_named_state_set_name(&v,"Workspace")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_named_state_validate(&v)==UMI_STATUS_OK);
    CHECK(umi_project_workspace_named_state_same_identity(&v,&w));
    CHECK(strcmp(umi_project_workspace_state_text(v.state),"ready")==0);
    return 0;
}
