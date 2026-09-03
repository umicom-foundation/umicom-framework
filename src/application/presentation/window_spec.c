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
        spec->minimum_width < 480U || spec->minimum_height < 320U ||
        spec->minimum_width > spec->initial_width ||
        spec->minimum_height > spec->initial_height ||
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

/* Choose the largest comfortable size that fits the current monitor while
 * keeping the minimum readable size as an explicit contract. */
UmiStatus umi_application_presentation_window_spec_fit(
    const UmiApplicationPresentationWindowSpec *spec,
    uint32_t available_width,
    uint32_t available_height,
    uint32_t *out_width,
    uint32_t *out_height,
    int *out_compact)
{
    UmiStatus status;

    if (out_width == NULL || out_height == NULL || out_compact == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_presentation_window_spec_validate(spec);
    if (status != UMI_STATUS_OK)
        return status;
    /* A frontend cannot make a governed workspace readable when its available
     * area is smaller than the declared minimum, so report that clearly. */
    if (available_width < spec->minimum_width || available_height < spec->minimum_height)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_width = available_width < spec->initial_width ? available_width : spec->initial_width;
    *out_height = available_height < spec->initial_height ? available_height : spec->initial_height;
    *out_compact = *out_width < spec->initial_width || *out_height < spec->initial_height;
    return UMI_STATUS_OK;
}
