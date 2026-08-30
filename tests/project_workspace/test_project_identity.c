/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_identity.c
 *
 * PURPOSE:
 *   Implement the test project identity behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_identity.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceProjectIdentity a,b;
    CHECK(umi_project_workspace_project_identity_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_identity_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_identity_set_name(&a,"Project Identity")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_identity_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_identity_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_identity_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_identity_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Identity")==0);
    return 0;
}
