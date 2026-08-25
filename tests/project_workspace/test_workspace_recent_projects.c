#include "umicom/project/workspace/workspace_recent_projects.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceRecentProjects v;
    umi_project_workspace_workspace_recent_projects_init(&v);
    CHECK(umi_project_workspace_workspace_recent_projects_add(&v,"one")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_recent_projects_contains(&v,"one"));
    CHECK(umi_project_workspace_workspace_recent_projects_remove(&v,"one")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_workspace_recent_projects_contains(&v,"one"));
    return 0;
}
