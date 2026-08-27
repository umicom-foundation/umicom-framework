/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_types.c
 *
 * PURPOSE:
 *   Verify define stable debugger-workbench identifiers, source locations, phases, commands and shared value types.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/types.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)
#include <string.h>

int main(void)
{
    UmiDebugWorkbenchEntry entry;
    UmiDebugWorkbenchRange range = {{9U, 7U}, {3U, 2U}};
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&entry, "session-1", "Session 1", "gdb-dap", "src/main.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_entry_valid(&entry));
    UMI_TEST_CHECK(entry.location.range.start.line == 3U);
    UMI_TEST_CHECK(strcmp(entry.location.path, "src/main.c") == 0);
    UMI_TEST_CHECK(umi_debug_workbench_session_transition_allowed(UMI_DEBUG_WORKBENCH_SESSION_RUNNING, UMI_DEBUG_WORKBENCH_SESSION_PAUSED));
    UMI_TEST_CHECK(!umi_debug_workbench_session_transition_allowed(UMI_DEBUG_WORKBENCH_SESSION_RUNNING, UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING));
    UMI_TEST_CHECK(umi_debug_workbench_command_bit(UMI_DEBUG_WORKBENCH_COMMAND_STEP_IN) != 0U);
    return 0;
}
