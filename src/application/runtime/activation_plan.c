/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/activation_plan.c
 *
 * PURPOSE:
 *   Resolve each layout panel against an optional Framework capability probe.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/activation_plan.h"

#include <string.h>

static int has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

UmiStatus umi_application_activation_plan_build(
    const UmiApplicationExperienceDefinition *experience,
    const char *layout_id,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationActivationPlan *out_plan)
{
    const UmiExperienceLayoutDefinition *layout;
    size_t index;
    if (experience == NULL || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_application_experience_layout_find(
        experience, layout_id != NULL ? layout_id : experience->default_layout_id);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (layout->panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(out_plan, 0, sizeof(*out_plan));
    out_plan->structure_size = sizeof(*out_plan);
    out_plan->layout = layout;
    for (index = 0U; index < layout->panel_count; ++index) {
        UmiApplicationActivationEntry *entry = &out_plan->entries[out_plan->entry_count++];
        entry->panel = umi_application_experience_panel_find(experience, layout->panel_ids[index]);
        if (entry->panel == NULL) return UMI_STATUS_NOT_FOUND;
        entry->required = (entry->panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U;
        entry->capability_available = !has_text(entry->panel->required_capability) ||
            probe == NULL || probe(entry->panel->required_capability, user_data) != 0;
        if (!entry->capability_available && entry->required) out_plan->blocked_count += 1U;
    }
    return UMI_STATUS_OK;
}

int umi_application_activation_plan_ready(const UmiApplicationActivationPlan *plan)
{
    return plan != NULL && plan->blocked_count == 0U;
}
