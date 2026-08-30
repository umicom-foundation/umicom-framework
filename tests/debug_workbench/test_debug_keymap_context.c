/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_keymap_context.c
 *
 * PURPOSE:
 *   Verify represent debugger keymap activation state and command-context precedence.
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
#include "umicom/debug/workbench/debug_keymap_context.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDebugKeymapContext model;
    umi_debug_workbench_debug_keymap_context_init(&model);
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_set_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_is_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE));
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_set_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE, false) == UMI_STATUS_OK);
    UMI_TEST_CHECK(!umi_debug_workbench_debug_keymap_context_is_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE));
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_set_primary(&model, UMI_DEBUG_WORKBENCH_COMMAND_STEP_OVER) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_valid(&model));
    return 0;
}
