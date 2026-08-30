/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_access_policy.c
 *
 * PURPOSE:
 *   Verify role, lock, read-only and lease evidence produce explicit edit
 *   decisions.
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
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerLeaseView lease;
    UmiWorkbenchDesignerAccessDecision decision;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_lease_view_init(&lease);
    TEST_REQUIRE_STATUS(umi_workbench_designer_lease_view_update(
        &lease, "layout.fixture", "user.sammy", "client.desktop",
        10U, 1000U, 5000U, true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_access_evaluate(
        &document, UMI_WORKBENCH_LAYOUT_ROLE_EDITOR,
        &lease, "user.sammy", "client.desktop", false, &decision));
    TEST_REQUIRE(decision.can_edit);
    TEST_REQUIRE(decision.reason == UMI_WORKBENCH_DESIGNER_ACCESS_GRANTED);

    TEST_REQUIRE_STATUS(umi_workbench_layout_document_set_flag(
        &document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED, true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_access_evaluate(
        &document, UMI_WORKBENCH_LAYOUT_ROLE_EDITOR,
        &lease, "user.sammy", "client.desktop", false, &decision));
    TEST_REQUIRE(!decision.can_edit);
    TEST_REQUIRE(decision.reason ==
        UMI_WORKBENCH_DESIGNER_ACCESS_LAYOUT_LOCKED);
    TEST_REQUIRE_STATUS(umi_workbench_designer_access_evaluate(
        &document, UMI_WORKBENCH_LAYOUT_ROLE_ADMINISTRATOR,
        &lease, "administrator", "admin.client", true, &decision));
    TEST_REQUIRE(decision.can_edit);
    TEST_REQUIRE(decision.administrator_override_used);
    return 0;
}
