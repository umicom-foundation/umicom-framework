/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/activation_plan.c
 *
 * PURPOSE:
 *   Resolve each layout panel against an optional Framework capability probe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/activation_plan.h"

#include <string.h>

/* Provide the has text operation used by this module and its client applications. */
static int has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

/*
 * Provide the application activation plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_activation_plan_build(
    const UmiApplicationExperienceDefinition *experience,
    const char *layout_id,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationActivationPlan *out_plan)
{
    const UmiExperienceLayoutDefinition *layout;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_application_experience_layout_find(
        experience, layout_id != NULL ? layout_id : experience->default_layout_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (layout->panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(out_plan, 0, sizeof(*out_plan));
    out_plan->structure_size = sizeof(*out_plan);
    out_plan->layout = layout;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->panel_count; ++index) {
        UmiApplicationActivationEntry *entry = &out_plan->entries[out_plan->entry_count++];
        entry->panel = umi_application_experience_panel_find(experience, layout->panel_ids[index]);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry->panel == NULL) return UMI_STATUS_NOT_FOUND;
        entry->required = (entry->panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U;
        entry->capability_available = !has_text(entry->panel->required_capability) ||
            probe == NULL || probe(entry->panel->required_capability, user_data) != 0;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!entry->capability_available && entry->required) out_plan->blocked_count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application activation plan ready operation used by this module and its
 * client applications.
 */
int umi_application_activation_plan_ready(const UmiApplicationActivationPlan *plan)
{
    return plan != NULL && plan->blocked_count == 0U;
}
