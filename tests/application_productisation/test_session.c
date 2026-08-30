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
#include <assert.h>
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

    assert(umi_product_application_session_init(
        &adoption, &session) == UMI_STATUS_OK);
    assert(umi_product_application_session_snapshot(
        &session, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.application_id, "org.umicom.studio") == 0);
    assert(snapshot.feature_count > 0U);
    assert(snapshot.panel_count > 0U);
    assert(snapshot.layout_count > 0U);
    assert(snapshot.workspace.active_panel_count > 0U);
    assert(snapshot.readiness_percent <= 100U);
    assert(snapshot.runnable);
    assert(snapshot.acceptance_ready);

    assert(umi_product_application_session_execute(
        &session, &command) == UMI_STATUS_OK);
    command.kind = UMI_PRODUCT_SESSION_SET_CONTEXT;
    command.target_id = "selection.active";
    command.value = "welcome";
    assert(umi_product_application_session_execute(
        &session, &command) == UMI_STATUS_OK);
    command.kind = UMI_PRODUCT_SESSION_REFRESH_READINESS;
    command.target_id = NULL;
    command.value = NULL;
    assert(umi_product_application_session_execute(
        &session, &command) == UMI_STATUS_OK);

    assert(umi_product_application_session_snapshot(
        &session, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.workspace.layout_locked);
    assert(snapshot.command_count == 3U);
    assert(snapshot.successful_command_count == 3U);
    assert(snapshot.failed_command_count == 0U);
    assert(snapshot.operation_count >= 2U);
    assert(umi_product_application_session_reset(&session) == UMI_STATUS_OK);
    assert(session.command_count == 0U);
    return 0;
}
