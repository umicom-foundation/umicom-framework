/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_session_state_model.c
 *
 * PURPOSE:
 *   Verify track debugger lifecycle state and enforce legal high-level phase transitions.
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
#include "umicom/debug/workbench/debug_session_state_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugWorkbenchDebugSessionStateModel model;
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_init(&model, "session-main") == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_RUNNING) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_PAUSED) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_record_stop(&model) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.stop_sequence == 1U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_valid(&model));
    return 0;
}
