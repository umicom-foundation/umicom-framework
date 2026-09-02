/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/window_spec.c
 *
 * PURPOSE:
 *   Validate portable application window geometry, behavior and recipe
 *   ownership before a frontend creates native windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/window_spec.h"

#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"

/*
 * Check that application presentation window boolean satisfies its contract before another
 * service relies on it.
 */
static int umi_application_presentation_window_boolean_valid(int value)
{
    return value == 0 || value == 1;
}

/*
 * Check that application presentation window spec satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_presentation_window_spec_validate(
    const UmiApplicationPresentationWindowSpec *spec)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL || spec->struct_size != sizeof(*spec) ||
        spec->api_version != UMI_APPLICATION_PRESENTATION_API_VERSION ||
        spec->recipe_id == NULL || spec->recipe_id[0] == '\0' ||
        spec->window_id == NULL || strncmp(spec->window_id, "org.umicom.window.", 18U) != 0 ||
        spec->density < UMI_APPLICATION_PRESENTATION_DENSITY_COMFORTABLE ||
        spec->density > UMI_APPLICATION_PRESENTATION_DENSITY_COMPACT ||
        spec->navigation < UMI_APPLICATION_PRESENTATION_NAVIGATION_EXPANDED ||
        spec->navigation > UMI_APPLICATION_PRESENTATION_NAVIGATION_HIDDEN ||
        spec->initial_width < 640U || spec->initial_height < 480U ||
        spec->navigation_width > spec->initial_width ||
        spec->side_panel_width > spec->initial_width ||
        spec->bottom_panel_height > spec->initial_height ||
        !umi_application_presentation_window_boolean_valid(spec->show_command_bar) ||
        !umi_application_presentation_window_boolean_valid(spec->show_status_bar) ||
        !umi_application_presentation_window_boolean_valid(spec->restore_session) ||
        !umi_application_presentation_window_boolean_valid(spec->allow_multiple_windows)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_component_recipe_catalogue_find(spec->recipe_id) != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_NOT_FOUND;
}
