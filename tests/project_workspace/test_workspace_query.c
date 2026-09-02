/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_query.c
 *
 * PURPOSE:
 *   Implement the test workspace query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_query.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceWorkspaceQuery v;
    CHECK(umi_project_workspace_workspace_query_init(&v,"id","workspace.changed","{}") == UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_query_validate(&v)==UMI_STATUS_OK);
    return 0;
}
