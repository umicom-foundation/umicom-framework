/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_trust_bridge.c
 *
 * PURPOSE:
 *   Implement the test workspace trust bridge behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_trust_bridge.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceTrustBridge a,b;
    CHECK(umi_project_workspace_workspace_trust_bridge_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_trust_bridge_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_trust_bridge_set_name(&a,"Workspace Trust Bridge")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_trust_bridge_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_workspace_trust_bridge_set_metric(&a,7U);
    CHECK(umi_project_workspace_workspace_trust_bridge_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_workspace_trust_bridge_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Workspace Trust Bridge")==0);
    return 0;
}
