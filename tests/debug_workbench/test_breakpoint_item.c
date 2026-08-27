/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_breakpoint_item.c
 *
 * PURPOSE:
 *   Verify represent one source, function, data, exception, instruction or log breakpoint.
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
#include "umicom/debug/workbench/breakpoint_item.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchBreakpointItem model;
    UmiDebugWorkbenchRange range = {{10U, 1U}, {10U, 1U}};
    UMI_TEST_CHECK(umi_debug_workbench_breakpoint_item_init(&model, "bp-main", UMI_DEBUG_WORKBENCH_BREAKPOINT_SOURCE, "src/main.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_breakpoint_item_set_verification(&model, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_breakpoint_item_record_hit(&model) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.verified && model.hit_count == 1U);
    UMI_TEST_CHECK(umi_debug_workbench_breakpoint_item_valid(&model));
    return 0;
}
