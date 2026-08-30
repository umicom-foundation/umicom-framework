/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_session_snapshot.c
 *
 * PURPOSE:
 *   Verify capture durable high-level debugger session state for workbench restoration.
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
#include "umicom/debug/workbench/debug_session_snapshot.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDebugSessionSnapshot model;
    umi_debug_workbench_debug_session_snapshot_init(&model);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_snapshot_capture(&model, "session-a", "frame-2", 2U, 5U, 3U, 4U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.session_count == 2U && model.breakpoint_count == 5U);
    UMI_TEST_CHECK(model.thread_count == 3U && model.watch_count == 4U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_snapshot_valid(&model));
    return 0;
}
