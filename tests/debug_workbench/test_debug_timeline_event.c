/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_timeline_event.c
 *
 * PURPOSE:
 *   Verify represent one timestamped debugger lifecycle, stop or navigation event.
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
#include "umicom/debug/workbench/debug_timeline_event.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDebugTimelineEvent model;
    UMI_TEST_CHECK(umi_debug_workbench_debug_timeline_event_init(&model, "event-1", 1U, 1000U, UMI_DEBUG_WORKBENCH_SESSION_PAUSED, "Breakpoint hit") == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_timeline_event_retime(&model, 1200U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.timestamp_ns == 1200U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_timeline_event_valid(&model));
    return 0;
}
