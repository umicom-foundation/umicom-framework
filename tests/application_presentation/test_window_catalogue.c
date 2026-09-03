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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationPresentationWindowSpec *window;
    uint32_t fitted_width = 0U;
    uint32_t fitted_height = 0U;
    int compact = 0;

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
    assert(window->minimum_width == 800U);
    assert(window->minimum_height == 600U);
    assert(window->show_command_bar);
    assert(window->show_status_bar);
    assert(umi_application_presentation_window_spec_validate(window) == UMI_STATUS_OK);
    /* A smaller monitor receives a safe fitted size and tells the frontend to
     * apply each panel's compact policy. */
    assert(umi_application_presentation_window_spec_fit(
               window, 1280U, 720U, &fitted_width, &fitted_height, &compact) ==
           UMI_STATUS_OK);
    assert(fitted_width == 1280U);
    assert(fitted_height == 720U);
    assert(compact);
    assert(umi_application_presentation_window_spec_fit(
               window, 640U, 480U, &fitted_width, &fitted_height, &compact) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
