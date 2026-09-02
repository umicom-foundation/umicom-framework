/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_cmake_source_group.c
 *
 * PURPOSE:
 *   Implement the test cmake source group behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_source_group.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceCmakeSourceGroup v;
    umi_project_workspace_cmake_source_group_init(&v);
    CHECK(umi_project_workspace_cmake_source_group_add(&v,"one")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_cmake_source_group_contains(&v,"one"));
    CHECK(umi_project_workspace_cmake_source_group_remove(&v,"one")==UMI_STATUS_OK);
    CHECK(!umi_project_workspace_cmake_source_group_contains(&v,"one"));
    return 0;
}
