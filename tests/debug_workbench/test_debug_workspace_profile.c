/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_workspace_profile.c
 *
 * PURPOSE:
 *   Verify persist per-workspace debugger layout and presentation preferences.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/debug_workspace_profile.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDebugWorkspaceProfile model;
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_init(&model, "workspace-main") == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_set_view_visible(&model, UMI_DEBUG_WORKBENCH_VIEW_MEMORY, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_view_visible(&model, UMI_DEBUG_WORKBENCH_VIEW_MEMORY));
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_set_primary_view(&model, UMI_DEBUG_WORKBENCH_VIEW_CALL_STACK) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_valid(&model));
    return 0;
}
