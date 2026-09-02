/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/panel_catalogue_view.c
 *
 * PURPOSE:
 *   Expose canonical panel projections without product-local panel catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/panel_catalogue_view.h"

/*
 * Return the number of records represented by application ui panel without changing their
 * state.
 */
size_t umi_application_ui_panel_count(const UmiApplicationExperienceDefinition *experience)
{
    return experience != NULL ? experience->panel_count : 0U;
}

/*
 * Provide the application ui panel row operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_ui_panel_row(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationPanelProjection *out_row)
{
    return umi_application_panel_projection_at(experience, index, out_row);
}
