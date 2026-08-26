/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/layout_selector_view.c
 *
 * PURPOSE:
 *   Expose canonical layout projections for toolkit adapters and thin products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/layout_selector_view.h"

size_t umi_application_ui_layout_count(const UmiApplicationExperienceDefinition *experience)
{
    return experience != NULL ? experience->layout_count : 0U;
}

UmiStatus umi_application_ui_layout_row(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationLayoutProjection *out_row)
{
    return umi_application_layout_projection_at(experience, index, out_row);
}
