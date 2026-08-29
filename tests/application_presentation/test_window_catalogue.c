/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_window_catalogue.c
 *
 * PURPOSE:
 *   Verify every workspace recipe has one portable window and each application
 *   exposes Learning, Standard and Focus choices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"

#include <assert.h>

#include "umicom/application/component/recipe_catalogue.h"

int main(void)
{
    const UmiApplicationPresentationWindowSpec *window;

    assert(umi_application_presentation_window_catalogue_count() ==
           umi_application_component_recipe_catalogue_count());
    assert(umi_application_presentation_window_catalogue_application_count(
               "org.umicom.studio") == 3U);
    window = umi_application_presentation_window_catalogue_find(
        "org.umicom.workspace.studio.standard");
    assert(window != NULL);
    assert(window->density == UMI_APPLICATION_PRESENTATION_DENSITY_BALANCED);
    assert(window->navigation == UMI_APPLICATION_PRESENTATION_NAVIGATION_EXPANDED);
    assert(window->initial_width >= 1600U);
    assert(window->show_command_bar);
    assert(window->show_status_bar);
    assert(umi_application_presentation_window_spec_validate(window) == UMI_STATUS_OK);
    return 0;
}
