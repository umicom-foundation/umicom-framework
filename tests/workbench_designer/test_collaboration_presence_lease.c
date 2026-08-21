/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_collaboration_presence_lease.c
 *
 * PURPOSE:
 *   Verify collaborator upserts, heartbeats, expiry and local or remote edit-
 *   lease projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerCollaborationModel model;
    UmiWorkbenchDesignerCollaborator collaborator;
    UmiWorkbenchDesignerPresencePolicy policy;
    UmiWorkbenchDesignerLeaseView lease;
    const UmiWorkbenchDesignerCollaborator *found;
    size_t expired;

    umi_workbench_designer_collaboration_init(&model);
    (void)memset(&collaborator, 0, sizeof(collaborator));
    TEST_REQUIRE_STATUS(test_copy_text(
        collaborator.user_id, sizeof(collaborator.user_id), "user.sammy"));
    TEST_REQUIRE_STATUS(test_copy_text(
        collaborator.client_id, sizeof(collaborator.client_id), "client.desktop"));
    TEST_REQUIRE_STATUS(test_copy_text(
        collaborator.display_name, sizeof(collaborator.display_name), "Sammy"));
    collaborator.state = UMI_WORKBENCH_DESIGNER_COLLABORATOR_EDITING;
    collaborator.connected_at_ms = 1000U;
    collaborator.last_activity_ms = 1000U;
    collaborator.local_user = true;
    TEST_REQUIRE_STATUS(umi_workbench_designer_collaboration_upsert(
        &model, &collaborator));
    TEST_REQUIRE(umi_workbench_designer_collaboration_editing_count(
        &model) == 1U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_presence_heartbeat(
        &model, "user.sammy", "client.desktop", "editor", true, 1500U));
    found = umi_workbench_designer_collaboration_find(
        &model, "user.sammy", "client.desktop");
    TEST_REQUIRE(found != NULL);
    TEST_REQUIRE(strcmp(found->active_node_id, "editor") == 0);
    policy = umi_workbench_designer_presence_policy_default();
    expired = umi_workbench_designer_presence_expire(
        &model, &policy, 1500U + policy.offline_after_ms + 1U);
    TEST_REQUIRE(expired <= 1U);

    umi_workbench_designer_lease_view_init(&lease);
    TEST_REQUIRE_STATUS(umi_workbench_designer_lease_view_update(
        &lease, "layout.fixture", "user.sammy", "client.desktop",
        42U, 1000U, 5000U, true));
    TEST_REQUIRE(umi_workbench_designer_lease_view_editable(&lease));
    umi_workbench_designer_lease_view_expire(&lease, 6000U);
    TEST_REQUIRE(!umi_workbench_designer_lease_view_editable(&lease));
    return 0;
}
