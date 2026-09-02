/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/experience_projection.c
 *
 * PURPOSE:
 *   Implement allocation-free experience projections suitable for any frontend adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/experience_projection.h"

#include <string.h>

/*
 * Find application panel projection while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_panel_projection_at(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationPanelProjection *out_projection)
{
    const UmiExperiencePanelDefinition *panel;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_projection == NULL || index >= experience->panel_count)
        return UMI_STATUS_INVALID_ARGUMENT;
    panel = &experience->panels[index];
    memset(out_projection, 0, sizeof(*out_projection));
    out_projection->panel_id = panel->panel_id;
    out_projection->title = panel->title;
    out_projection->region = panel->default_region;
    out_projection->context_group_id = panel->context_group_id;
    out_projection->flags = panel->flags;
    return UMI_STATUS_OK;
}

/*
 * Find application layout projection while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_layout_projection_at(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationLayoutProjection *out_projection)
{
    const UmiExperienceLayoutDefinition *layout;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_projection == NULL || index >= experience->layout_count)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = &experience->layouts[index];
    memset(out_projection, 0, sizeof(*out_projection));
    out_projection->layout_id = layout->layout_id;
    out_projection->title = layout->title;
    out_projection->panel_count = layout->panel_count;
    out_projection->flags = layout->flags;
    return UMI_STATUS_OK;
}

/*
 * Find application feature projection while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_application_feature_projection_at(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationFeatureProjection *out_projection)
{
    const UmiExperienceFeatureDefinition *feature;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_projection == NULL || index >= experience->feature_count)
        return UMI_STATUS_INVALID_ARGUMENT;
    feature = &experience->features[index];
    memset(out_projection, 0, sizeof(*out_projection));
    out_projection->feature_id = feature->feature_id;
    out_projection->title = feature->title;
    out_projection->state = feature->state;
    out_projection->priority = feature->priority;
    out_projection->owner = feature->owner;
    return UMI_STATUS_OK;
}
