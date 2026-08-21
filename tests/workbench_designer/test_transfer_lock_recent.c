/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_transfer_lock_recent.c
 *
 * PURPOSE:
 *   Verify portable file planning, lock state and recently used layout ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerTransferPlan plan;
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerLockState lock_state;
    UmiWorkbenchDesignerRecentList recent;
    const UmiWorkbenchDesignerRecentItem *item;

    umi_workbench_designer_transfer_plan_init(
        &plan, UMI_WORKBENCH_DESIGNER_TRANSFER_EXPORT, "export.fixture");
    TEST_REQUIRE_STATUS(test_copy_text(
        plan.layout_id, sizeof(plan.layout_id), "layout.fixture"));
    TEST_REQUIRE_STATUS(test_copy_text(
        plan.path, sizeof(plan.path), "C:/layouts/fixture.umilayout"));
    TEST_REQUIRE_STATUS(test_copy_text(
        plan.actor_id, sizeof(plan.actor_id), "user.sammy"));
    plan.validate_schema = true;
    TEST_REQUIRE_STATUS(umi_workbench_designer_transfer_plan_validate(&plan));
    TEST_REQUIRE(umi_workbench_designer_path_is_layout(plan.path));
    TEST_REQUIRE(!umi_workbench_designer_path_is_layout("fixture.ui"));

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_lock_state_build(&lock_state, &document, false);
    TEST_REQUIRE(!lock_state.locked);
    TEST_REQUIRE_STATUS(umi_workbench_layout_document_set_flag(
        &document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED, true));
    umi_workbench_designer_lock_state_build(&lock_state, &document, false);
    TEST_REQUIRE(lock_state.locked);

    umi_workbench_designer_recent_init(&recent);
    TEST_REQUIRE_STATUS(umi_workbench_designer_recent_touch(
        &recent, "layout.fixture", "Fixture Layout", 1000U, 1U));
    TEST_REQUIRE_STATUS(umi_workbench_designer_recent_touch(
        &recent, "layout.trading", "Trading", 2000U, 2U));
    item = umi_workbench_designer_recent_at(&recent, 0U);
    TEST_REQUIRE(item != NULL);
    TEST_REQUIRE(strcmp(item->layout_id, "layout.trading") == 0);
    return 0;
}
