/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_launch_request.c
 *
 * PURPOSE:
 *   Verify capture a validated workbench launch request before dispatch to the canonical debug runtime.
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
#include "umicom/debug/workbench/debug_launch_request.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDebugLaunchRequest model;
    UmiDebugWorkbenchRange range = {{4U, 2U}, {4U, 9U}};
    UMI_TEST_CHECK(umi_debug_workbench_debug_launch_request_init(&model, "debug_launch_request-1", "DebugLaunchRequest", "debug-workbench", "src/example.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_launch_request_set_state(&model, 7U, 42U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_launch_request_set_selected(&model, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_launch_request_set_enabled(&model, false) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.value.state == 7U);
    UMI_TEST_CHECK(model.value.value == 42U);
    UMI_TEST_CHECK(model.selected);
    UMI_TEST_CHECK(!model.enabled);
    UMI_TEST_CHECK(umi_debug_workbench_debug_launch_request_valid(&model));
    return 0;
}
