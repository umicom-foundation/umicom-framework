/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_conformance.c
 *
 * PURPOSE:
 *   Verify evaluate required debugger-workbench capabilities for frontend conformance.
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
#include "umicom/debug/workbench/debug_conformance.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDebugConformance model;
    uint64_t required = UMI_DEBUG_WORKBENCH_CONFORMANCE_SESSIONS | UMI_DEBUG_WORKBENCH_CONFORMANCE_BREAKPOINTS | UMI_DEBUG_WORKBENCH_CONFORMANCE_INSPECTION;
    umi_debug_workbench_debug_conformance_init(&model, required);
    UMI_TEST_CHECK(!umi_debug_workbench_debug_conformance_passes(&model));
    UMI_TEST_CHECK(umi_debug_workbench_debug_conformance_set_available(&model, required, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_conformance_passes(&model));
    return 0;
}
