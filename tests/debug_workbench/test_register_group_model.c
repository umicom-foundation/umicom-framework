/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_register_group_model.c
 *
 * PURPOSE:
 *   Verify represent one architecture register group and its display state.
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
#include "umicom/debug/workbench/register_group_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

int main(void)
{
    UmiDebugWorkbenchRegisterGroupModel model;
    UmiDebugWorkbenchRange range = {{4U, 2U}, {4U, 9U}};
    UMI_TEST_CHECK(umi_debug_workbench_register_group_model_init(&model, "register_group_model-1", "RegisterGroupModel", "debug-workbench", "src/example.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_register_group_model_set_state(&model, 7U, 42U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_register_group_model_set_selected(&model, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_register_group_model_set_enabled(&model, false) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.value.state == 7U);
    UMI_TEST_CHECK(model.value.value == 42U);
    UMI_TEST_CHECK(model.selected);
    UMI_TEST_CHECK(!model.enabled);
    UMI_TEST_CHECK(umi_debug_workbench_register_group_model_valid(&model));
    return 0;
}
