/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_behavior_catalogue.c
 *
 * PURPOSE:
 *   Verify that every reusable panel has one valid live behavior profile and
 *   that important Studio and Trader panels carry the intended semantics.
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

#include "umicom/application/presentation/presentation.h"

int main(void)
{
    const UmiApplicationPresentationSurfaceBehavior *editor;
    const UmiApplicationPresentationSurfaceBehavior *chart;
    const UmiApplicationPresentationSurfaceBehavior *ticket;

    assert(umi_application_presentation_surface_behavior_catalogue_validate() ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_behavior_catalogue_count() ==
           umi_application_presentation_panel_catalogue_count());

    editor = umi_application_presentation_surface_behavior_catalogue_find(
        "umicom.development.editor");
    assert(editor != NULL);
    assert(editor->refresh_policy == UMI_APPLICATION_PRESENTATION_REFRESH_ON_FOCUS);
    assert(editor->persistence_policy ==
           UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE);
    assert(editor->command_mode == UMI_APPLICATION_PRESENTATION_COMMAND_EDITABLE);
    assert(strcmp(editor->context_group_id, "umicom.context.source") == 0);

    chart = umi_application_presentation_surface_behavior_catalogue_find(
        "umicom.trading.chart");
    assert(chart != NULL);
    assert(chart->refresh_policy == UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING);
    assert(chart->refresh_interval_seconds == 1U);
    assert(chart->command_mode == UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY);

    ticket = umi_application_presentation_surface_behavior_catalogue_find(
        "umicom.trading.order-ticket");
    assert(ticket != NULL);
    assert(ticket->command_mode == UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED);
    assert(ticket->accept_context);
    return 0;
}
