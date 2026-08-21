/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_collaboration_cursor.c
 *
 * PURPOSE:
 *   Verify remote cursor upsert, stale sequence rejection and heartbeat expiry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerRemoteCursorModel model;
    UmiWorkbenchDesignerRemoteCursor cursor;
    umi_workbench_designer_remote_cursor_model_init(&model);
    (void)memset(&cursor, 0, sizeof(cursor));
    TEST_REQUIRE_STATUS(test_copy_text(cursor.user_id, sizeof(cursor.user_id), "user.alex"));
    TEST_REQUIRE_STATUS(test_copy_text(cursor.client_id, sizeof(cursor.client_id), "client.one"));
    TEST_REQUIRE_STATUS(test_copy_text(cursor.display_name, sizeof(cursor.display_name), "Alex"));
    cursor.world_position = (UmiWorkbenchDesignerPoint){100.0, 120.0};
    cursor.sequence = 2U;
    cursor.last_update_ms = 1000U;
    cursor.visible = true;
    TEST_REQUIRE_STATUS(umi_workbench_designer_remote_cursor_upsert(&model, &cursor));
    cursor.sequence = 1U;
    TEST_REQUIRE(umi_workbench_designer_remote_cursor_upsert(&model, &cursor) ==
                 UMI_STATUS_INVALID_STATE);
    TEST_REQUIRE(umi_workbench_designer_remote_cursor_prune(&model, 5000U, 1000U) == 1U);
    TEST_REQUIRE(model.count == 0U);
    return 0;
}
