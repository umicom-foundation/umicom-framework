/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_session.c
 *
 * PURPOSE:
 *   Verify one product session controls canonical layouts, panels, context and
 *   readiness without introducing application-owned runtime implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_runtime/check.h"
#include <string.h>

#include "umicom/application/productisation/session.h"

int main(void)
{
    const UmiProductApplicationAdoption adoption = {
        sizeof(UmiProductApplicationAdoption),
        "studio",
        "org.umicom.studio",
        "Umicom Studio IDE",
        "umicom-studio-ide",
        UMI_PRODUCT_FRONTEND_FLAG_CONSOLE | UMI_PRODUCT_FRONTEND_FLAG_GTK4,
        1, 1, 1, 1
    };
    UmiProductApplicationSession session;
    UmiProductApplicationSessionSnapshot snapshot;
    UmiProductApplicationSessionCommand command = {
        sizeof(UmiProductApplicationSessionCommand),
        UMI_PRODUCT_SESSION_SET_LAYOUT_LOCKED,
        NULL,
        NULL,
        true
    };

    UMI_TEST_REQUIRE(umi_product_application_session_init(
        &adoption, &session) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_application_session_snapshot(
        &session, &snapshot) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(snapshot.application_id, "org.umicom.studio") == 0);
    UMI_TEST_REQUIRE(snapshot.feature_count > 0U);
    UMI_TEST_REQUIRE(snapshot.panel_count > 0U);
    UMI_TEST_REQUIRE(snapshot.layout_count > 0U);
    UMI_TEST_REQUIRE(snapshot.workspace.active_panel_count > 0U);
    UMI_TEST_REQUIRE(snapshot.readiness_percent <= 100U);
    UMI_TEST_REQUIRE(snapshot.runnable);
    UMI_TEST_REQUIRE(snapshot.acceptance_ready);

    UMI_TEST_REQUIRE(umi_product_application_session_execute(
        &session, &command) == UMI_STATUS_OK);
    command.kind = UMI_PRODUCT_SESSION_SET_CONTEXT;
    command.target_id = "selection.active";
    command.value = "welcome";
    UMI_TEST_REQUIRE(umi_product_application_session_execute(
        &session, &command) == UMI_STATUS_OK);
    command.kind = UMI_PRODUCT_SESSION_REFRESH_READINESS;
    command.target_id = NULL;
    command.value = NULL;
    UMI_TEST_REQUIRE(umi_product_application_session_execute(
        &session, &command) == UMI_STATUS_OK);

    UMI_TEST_REQUIRE(umi_product_application_session_snapshot(
        &session, &snapshot) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(snapshot.workspace.layout_locked);
    UMI_TEST_REQUIRE(snapshot.command_count == 3U);
    UMI_TEST_REQUIRE(snapshot.successful_command_count == 3U);
    UMI_TEST_REQUIRE(snapshot.failed_command_count == 0U);
    UMI_TEST_REQUIRE(snapshot.operation_count >= 2U);
    UMI_TEST_REQUIRE(umi_product_application_session_reset(&session) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(session.command_count == 0U);
    return 0;
}
