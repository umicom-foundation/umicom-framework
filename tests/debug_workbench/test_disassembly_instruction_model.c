/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_disassembly_instruction_model.c
 *
 * PURPOSE:
 *   Verify represent one decoded instruction, address and source association.
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
#include "umicom/debug/workbench/disassembly_instruction_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchDisassemblyInstructionModel model;
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_instruction_model_init(&model, "range-1", UINT64_C(0x1000), UINT64_C(256)) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_instruction_model_contains(&model, UINT64_C(0x1080)));
    UMI_TEST_CHECK(!umi_debug_workbench_disassembly_instruction_model_contains(&model, UINT64_C(0x1200)));
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_instruction_model_set_cursor(&model, UINT64_C(0x10f0)) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_instruction_model_set_follow_execution(&model, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_instruction_model_valid(&model));
    return 0;
}
