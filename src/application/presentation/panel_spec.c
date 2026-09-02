/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_spec.c
 *
 * PURPOSE:
 *   Validate reusable presentation data against canonical component identity,
 *   multiplicity and fixed-capacity safety rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/panel_spec.h"

#include <string.h>

#include "umicom/application/component_catalogue.h"

/*
 * Check that application presentation boolean satisfies its contract before another
 * service relies on it.
 */
static int umi_application_presentation_boolean_valid(int value)
{
    return value == 0 || value == 1;
}

/*
 * Check that application presentation panel spec satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_presentation_panel_spec_validate(
    const UmiApplicationPresentationPanelSpec *spec)
{
    const UmiApplicationComponentDefinition *definition;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL || spec->struct_size != sizeof(*spec) ||
        spec->api_version != UMI_APPLICATION_PRESENTATION_API_VERSION ||
        spec->component_id == NULL || spec->component_id[0] == '\0' ||
        spec->icon_id == NULL || strncmp(spec->icon_id, "umicom.icon.", 12U) != 0 ||
        spec->surface_kind < UMI_APPLICATION_PRESENTATION_SURFACE_VIEW ||
        spec->surface_kind > UMI_APPLICATION_PRESENTATION_SURFACE_SERVICE ||
        spec->chrome < UMI_APPLICATION_PRESENTATION_CHROME_STANDARD ||
        spec->chrome > UMI_APPLICATION_PRESENTATION_CHROME_SERVICE ||
        spec->preferred_region < UMI_APPLICATION_COMPONENT_REGION_PRIMARY ||
        spec->preferred_region > UMI_APPLICATION_COMPONENT_REGION_FLOATING ||
        spec->minimum_width == 0U || spec->minimum_height == 0U ||
        spec->empty_action < UMI_APPLICATION_PRESENTATION_EMPTY_NONE ||
        spec->empty_action > UMI_APPLICATION_PRESENTATION_EMPTY_CONNECT ||
        !umi_application_presentation_boolean_valid(spec->allow_floating) ||
        !umi_application_presentation_boolean_valid(spec->allow_closing) ||
        !umi_application_presentation_boolean_valid(spec->allow_multiple) ||
        !umi_application_presentation_boolean_valid(spec->default_visible)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    definition = umi_application_component_catalogue_find(spec->component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (spec->allow_multiple != definition->multi_instance) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
