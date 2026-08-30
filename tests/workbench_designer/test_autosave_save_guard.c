/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_autosave_save_guard.c
 *
 * PURPOSE:
 *   Verify autosave due, active and completion transitions and close-decision
 *   evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerAutosavePolicy policy =
        umi_workbench_designer_autosave_policy_default();
    UmiWorkbenchDesignerAutosave autosave;
    UmiWorkbenchDesignerStatusModel status;
    UmiWorkbenchDesignerSaveGuardResult guard;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerCollaborationModel collaboration;

    policy.debounce_ms = 1000U;
    policy.maximum_delay_ms = 5000U;
    umi_workbench_designer_autosave_init(&autosave, &policy);
    umi_workbench_designer_autosave_mark_dirty(&autosave, 2000U);
    TEST_REQUIRE(!umi_workbench_designer_autosave_due(&autosave, 2500U));
    TEST_REQUIRE(umi_workbench_designer_autosave_due(&autosave, 3000U));
    TEST_REQUIRE_STATUS(umi_workbench_designer_autosave_begin(
        &autosave, 3000U));
    umi_workbench_designer_autosave_complete(
        &autosave, UMI_STATUS_OK, 3050U);
    TEST_REQUIRE(autosave.last_success_ms == 3050U);
    TEST_REQUIRE(!autosave.dirty);

    umi_workbench_designer_status_init(&status);
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_collaboration_init(&collaboration);
    TEST_REQUIRE_STATUS(umi_workbench_designer_status_update(
        &status, "Unsaved", "Fixture Layout",
        UMI_WORKBENCH_DESIGNER_MODE_DESIGN,
        UMI_WORKBENCH_DESIGNER_TOOL_SELECT,
        UMI_WORKBENCH_DESIGNER_SAVE_DIRTY,
        (UmiWorkbenchDesignerPoint){0.0, 0.0}, 1.0,
        &selection, &collaboration, 0U, false, 5U));
    TEST_REQUIRE_STATUS(umi_workbench_designer_save_guard_evaluate(
        &status, true, true, &guard));
    TEST_REQUIRE(guard.decision == UMI_WORKBENCH_DESIGNER_CLOSE_PROMPT_SAVE);
    TEST_REQUIRE(guard.must_preserve_recovery);
    return 0;
}
