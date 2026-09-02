/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_toolbar_state.c
 *
 * PURPOSE:
 *   Verify model debugger toolbar command availability independently of any GUI toolkit.
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
#include "umicom/debug/workbench/debug_toolbar_state.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugWorkbenchDebugToolbarState model;
    umi_debug_workbench_debug_toolbar_state_init(&model);
    UMI_TEST_CHECK(umi_debug_workbench_debug_toolbar_state_set_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_toolbar_state_is_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE));
    UMI_TEST_CHECK(umi_debug_workbench_debug_toolbar_state_set_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE, false) == UMI_STATUS_OK);
    UMI_TEST_CHECK(!umi_debug_workbench_debug_toolbar_state_is_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE));
    UMI_TEST_CHECK(umi_debug_workbench_debug_toolbar_state_set_primary(&model, UMI_DEBUG_WORKBENCH_COMMAND_STEP_OVER) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_toolbar_state_valid(&model));
    return 0;
}
