#include "umicom/project/workspace/workspace_event.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceEvent v;
    CHECK(umi_project_workspace_workspace_event_init(&v,"id","workspace.changed","{}") == UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_event_validate(&v)==UMI_STATUS_OK);
    return 0;
}
