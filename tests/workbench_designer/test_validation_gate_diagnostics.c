/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_validation_gate_diagnostics.c
 *
 * PURPOSE:
 *   Verify semantic and designer diagnostics block invalid preview, save or
 *   export actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerDiagnostics diagnostics;
    UmiWorkbenchDesignerValidationGate gate;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerLeaseView lease;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_lease_view_init(&lease);
    umi_workbench_designer_diagnostics_init(&diagnostics);
    TEST_REQUIRE_STATUS(umi_workbench_designer_diagnostics_build(
        &diagnostics, &document, &selection, &viewport, &lease,
        UMI_WORKBENCH_DESIGNER_SAVE_CLEAN));
    umi_workbench_designer_validation_gate_init(&gate);
    TEST_REQUIRE_STATUS(umi_workbench_designer_validation_gate_evaluate(
        &gate, &document, &diagnostics, true));
    TEST_REQUIRE(gate.can_preview);
    TEST_REQUIRE(gate.can_save);

    document.root_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    TEST_REQUIRE_STATUS(umi_workbench_designer_validation_gate_evaluate(
        &gate, &document, &diagnostics, true));
    TEST_REQUIRE(!gate.can_preview);
    TEST_REQUIRE(gate.error_count > 0U);
    return 0;
}
