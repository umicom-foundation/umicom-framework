/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_status_model.c
 *
 * PURPOSE:
 *   Verify aggregate active session, stop reason and inspection-count status for workbench chrome.
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
#include "umicom/debug/workbench/debug_status_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugWorkbenchDebugStatusModel model;
    UMI_TEST_CHECK(umi_debug_workbench_debug_status_model_init(&model, "session-status") == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_status_model_update(&model, UMI_DEBUG_WORKBENCH_SESSION_PAUSED, "breakpoint", 4U, 8U, 21U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.thread_count == 4U && model.frame_count == 8U && model.variable_count == 21U);
    UMI_TEST_CHECK(strcmp(model.stop_reason, "breakpoint") == 0);
    UMI_TEST_CHECK(umi_debug_workbench_debug_status_model_valid(&model));
    return 0;
}
