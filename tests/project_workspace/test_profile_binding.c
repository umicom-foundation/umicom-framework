/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_profile_binding.c
 *
 * PURPOSE:
 *   Implement the test profile binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/profile_binding.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceProfileBinding v;
    CHECK(umi_project_workspace_profile_binding_init(&v,"project","profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_profile_binding_matches(&v,"project","profile"));
    CHECK(umi_project_workspace_profile_binding_retarget(&v,"profile2")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_profile_binding_matches(&v,"project","profile2"));
    return 0;
}
