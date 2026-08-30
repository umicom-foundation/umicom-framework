/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_session.c
 *
 * PURPOSE:
 *   Implement the test workspace session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_session.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceSession a,b;
    CHECK(umi_project_workspace_workspace_session_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_session_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_session_set_name(&a,"Workspace Session")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_session_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_workspace_session_set_metric(&a,7U);
    CHECK(umi_project_workspace_workspace_session_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_workspace_session_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Workspace Session")==0);
    return 0;
}
