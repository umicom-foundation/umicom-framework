/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_call_stack_model.c
 *
 * PURPOSE:
 *   Verify maintain call-stack entries and the focused frame for the active thread.
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
#include "umicom/debug/workbench/call_stack_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchCallStackModel model;
    UmiDebugWorkbenchEntry first;
    UmiDebugWorkbenchEntry second;
    UmiDebugWorkbenchRange range = {{1U, 1U}, {1U, 3U}};
    umi_debug_workbench_call_stack_model_init(&model);
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&first, "first", "First", "", "src/first.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&second, "second", "Second", "", "src/second.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_call_stack_model_add(&model, &first) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_call_stack_model_add(&model, &second) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_call_stack_model_add(&model, &first) == UMI_STATUS_ALREADY_EXISTS);
    UMI_TEST_CHECK(umi_debug_workbench_call_stack_model_select(&model, "second") == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.active_index == 1U);
    UMI_TEST_CHECK(umi_debug_workbench_call_stack_model_remove(&model, "first") == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.active_index == 0U);
    UMI_TEST_CHECK(umi_debug_workbench_call_stack_model_find(&model, "second") != NULL);
    UMI_TEST_CHECK(umi_debug_workbench_call_stack_model_valid(&model));
    return 0;
}
