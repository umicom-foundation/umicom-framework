/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/panel_catalogue_view.c
 *
 * PURPOSE:
 *   Expose canonical panel projections without product-local panel catalogues.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/panel_catalogue_view.h"

size_t umi_application_ui_panel_count(const UmiApplicationExperienceDefinition *experience)
{
    return experience != NULL ? experience->panel_count : 0U;
}

UmiStatus umi_application_ui_panel_row(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationPanelProjection *out_row)
{
    return umi_application_panel_projection_at(experience, index, out_row);
}
