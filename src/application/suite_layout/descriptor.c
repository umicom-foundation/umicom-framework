/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/descriptor.c
 *
 * PURPOSE:
 *   Build application layout coverage directly from canonical experience panel and layout metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/descriptor.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    if (written < 0 || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_layout_descriptor_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSuiteLayoutDescriptor *out_descriptor)
{
    const UmiExperienceLayoutDefinition *layout;
    size_t index;
    UmiStatus status;
    if (experience == NULL || out_descriptor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_descriptor, 0, sizeof(*out_descriptor));
    status = copy_text(out_descriptor->application_id,
                       sizeof(out_descriptor->application_id),
                       experience->application_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_descriptor->display_name,
                       sizeof(out_descriptor->display_name),
                       experience->display_name);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_descriptor->default_layout_id,
                       sizeof(out_descriptor->default_layout_id),
                       experience->default_layout_id);
    if (status != UMI_STATUS_OK) return status;
    out_descriptor->panel_count = experience->panel_count;
    out_descriptor->layout_count = experience->layout_count;
    layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    out_descriptor->default_layout_valid = 1;
    out_descriptor->default_panel_count = layout->panel_count;
    for (index = 0U; index < layout->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(experience, layout->panel_ids[index]);
        UmiUiPlacement placement;
        if (panel == NULL) return UMI_STATUS_NOT_FOUND;
        status = umi_ui_placement_parse(panel->default_region, &placement);
        if (status != UMI_STATUS_OK) return status;
        switch (placement) {
        case UMI_UI_PLACEMENT_LEFT: out_descriptor->left_count++; break;
        case UMI_UI_PLACEMENT_RIGHT: out_descriptor->right_count++; break;
        case UMI_UI_PLACEMENT_TOP: out_descriptor->top_count++; break;
        case UMI_UI_PLACEMENT_BOTTOM: out_descriptor->bottom_count++; break;
        case UMI_UI_PLACEMENT_CENTRE: out_descriptor->centre_count++; break;
        case UMI_UI_PLACEMENT_FLOATING: out_descriptor->floating_count++; break;
        default: return UMI_STATUS_INVALID_STATE;
        }
    }
    out_descriptor->complete = out_descriptor->default_layout_valid &&
        out_descriptor->default_panel_count > 0U &&
        (out_descriptor->left_count + out_descriptor->right_count +
         out_descriptor->top_count + out_descriptor->bottom_count +
         out_descriptor->centre_count + out_descriptor->floating_count) ==
            out_descriptor->default_panel_count;
    return out_descriptor->complete ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
