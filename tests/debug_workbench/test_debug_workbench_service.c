/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_workbench_service.c
 *
 * PURPOSE:
 *   Verify compose canonical debug runtime state into one toolkit-neutral debugger workbench service.
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
#include "umicom/debug/workbench/debug_workbench_service.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugWorkbenchDebugWorkbenchService service;
    UmiDebugWorkbenchEntry session;
    UmiDebugWorkbenchRange range = {{0U, 0U}, {0U, 0U}};
    umi_debug_workbench_debug_workbench_service_init(&service);
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&session, "session-main", "Main debug session", "gdb-dap", NULL, range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_service_open_session(&service, &session) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_service_refresh_status(&service, UMI_DEBUG_WORKBENCH_SESSION_PAUSED, "breakpoint", 3U, 12U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(service.status.phase == UMI_DEBUG_WORKBENCH_SESSION_PAUSED);
    UMI_TEST_CHECK(service.status.frame_count == 3U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_service_valid(&service));
    return 0;
}
