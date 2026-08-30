/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_session_group.c
 *
 * PURPOSE:
 *   Verify group compound or related debug sessions under one workbench operation.
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
#include "umicom/debug/workbench/debug_session_group.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDebugSessionGroup model;
    UmiDebugWorkbenchEntry first;
    UmiDebugWorkbenchEntry second;
    UmiDebugWorkbenchRange range = {{1U, 1U}, {1U, 3U}};
    umi_debug_workbench_debug_session_group_init(&model);
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&first, "first", "First", "", "src/first.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&second, "second", "Second", "", "src/second.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_group_add(&model, &first) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_group_add(&model, &second) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_group_add(&model, &first) == UMI_STATUS_ALREADY_EXISTS);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_group_select(&model, "second") == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.active_index == 1U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_group_remove(&model, "first") == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.active_index == 0U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_group_find(&model, "second") != NULL);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_group_valid(&model));
    return 0;
}
