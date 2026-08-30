/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_application_frontend_selection.c
 *
 * PURPOSE:
 *   Implement the test application frontend selection behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/application_frontend_selection.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceApplicationFrontendSelection v;
    umi_project_workspace_application_frontend_selection_init(&v);
    CHECK(umi_project_workspace_application_frontend_selection_add(&v,"one")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_frontend_selection_contains(&v,"one"));
    CHECK(umi_project_workspace_application_frontend_selection_remove(&v,"one")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_application_frontend_selection_contains(&v,"one"));
    return 0;
}
