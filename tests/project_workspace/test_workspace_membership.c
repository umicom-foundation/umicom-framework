/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_membership.c
 *
 * PURPOSE:
 *   Implement the test workspace membership behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_membership.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceMembership v;
    CHECK(umi_project_workspace_workspace_membership_init(&v,"project","profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_membership_matches(&v,"project","profile"));
    CHECK(umi_project_workspace_workspace_membership_retarget(&v,"profile2")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_membership_matches(&v,"project","profile2"));
    return 0;
}
