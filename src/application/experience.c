/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience.c
 *
 * PURPOSE:
 *   Validate and query immutable application experience definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience.h"

#include <string.h>

static int text_present(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static int valid_feature_state(UmiExperienceFeatureState state)
{
    return state >= UMI_EXPERIENCE_FEATURE_PLANNED &&
           state <= UMI_EXPERIENCE_FEATURE_VERIFIED;
}

static int valid_priority(UmiExperiencePriority priority)
{
    return priority >= UMI_EXPERIENCE_PRIORITY_P0 &&
           priority <= UMI_EXPERIENCE_PRIORITY_P4;
}

static int valid_owner(UmiExperienceOwnership owner)
{
    return owner >= UMI_EXPERIENCE_OWNER_FRAMEWORK &&
           owner <= UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER;
}

const UmiExperiencePanelDefinition *umi_application_experience_panel_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *panel_id)
{
    size_t index;
    if (definition == NULL || !text_present(panel_id)) return NULL;
    for (index = 0U; index < definition->panel_count; ++index) {
        if (strcmp(definition->panels[index].panel_id, panel_id) == 0)
            return &definition->panels[index];
    }
    return NULL;
}

const UmiExperienceLayoutDefinition *umi_application_experience_layout_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *layout_id)
{
    size_t index;
    if (definition == NULL || !text_present(layout_id)) return NULL;
    for (index = 0U; index < definition->layout_count; ++index) {
        if (strcmp(definition->layouts[index].layout_id, layout_id) == 0)
            return &definition->layouts[index];
    }
    return NULL;
}

const UmiExperienceFeatureDefinition *umi_application_experience_feature_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *feature_id)
{
    size_t index;
    if (definition == NULL || !text_present(feature_id)) return NULL;
    for (index = 0U; index < definition->feature_count; ++index) {
        if (strcmp(definition->features[index].feature_id, feature_id) == 0)
            return &definition->features[index];
    }
    return NULL;
}

UmiStatus umi_application_experience_validate(
    const UmiApplicationExperienceDefinition *definition)
{
    size_t index;
    size_t nested;

    if (definition == NULL ||
        definition->structure_size != sizeof(*definition) ||
        definition->api_version != UMI_APPLICATION_EXPERIENCE_API_VERSION ||
        !text_present(definition->application_id) ||
        !text_present(definition->display_name) ||
        !text_present(definition->default_layout_id) ||
        definition->panels == NULL || definition->panel_count == 0U ||
        definition->layouts == NULL || definition->layout_count == 0U ||
        definition->features == NULL || definition->feature_count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U; index < definition->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel = &definition->panels[index];
        if (panel->structure_size != sizeof(*panel) ||
            !text_present(panel->panel_id) ||
            !text_present(panel->title) ||
            !text_present(panel->summary) ||
            !text_present(panel->default_region))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (nested = index + 1U; nested < definition->panel_count; ++nested) {
            if (strcmp(panel->panel_id,
                       definition->panels[nested].panel_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    for (index = 0U; index < definition->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout = &definition->layouts[index];
        if (layout->structure_size != sizeof(*layout) ||
            !text_present(layout->layout_id) ||
            !text_present(layout->title) ||
            !text_present(layout->description) ||
            layout->panel_ids == NULL || layout->panel_count == 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (nested = 0U; nested < layout->panel_count; ++nested) {
            if (umi_application_experience_panel_find(
                    definition, layout->panel_ids[nested]) == NULL)
                return UMI_STATUS_NOT_FOUND;
        }
        for (nested = index + 1U; nested < definition->layout_count; ++nested) {
            if (strcmp(layout->layout_id,
                       definition->layouts[nested].layout_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    if (umi_application_experience_layout_find(
            definition, definition->default_layout_id) == NULL)
        return UMI_STATUS_NOT_FOUND;

    for (index = 0U; index < definition->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &definition->features[index];
        if (feature->structure_size != sizeof(*feature) ||
            !text_present(feature->feature_id) ||
            !text_present(feature->title) ||
            !text_present(feature->summary) ||
            !valid_feature_state(feature->state) ||
            !valid_priority(feature->priority) ||
            !valid_owner(feature->owner))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (nested = index + 1U; nested < definition->feature_count; ++nested) {
            if (strcmp(feature->feature_id,
                       definition->features[nested].feature_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    return UMI_STATUS_OK;
}

size_t umi_application_experience_feature_state_count(
    const UmiApplicationExperienceDefinition *definition,
    UmiExperienceFeatureState state)
{
    size_t index;
    size_t count = 0U;
    if (definition == NULL || !valid_feature_state(state)) return 0U;
    for (index = 0U; index < definition->feature_count; ++index) {
        if (definition->features[index].state == state) ++count;
    }
    return count;
}

unsigned umi_application_experience_readiness_percent(
    const UmiApplicationExperienceDefinition *definition)
{
    size_t index;
    unsigned total = 0U;
    if (definition == NULL || definition->feature_count == 0U) return 0U;

    for (index = 0U; index < definition->feature_count; ++index) {
        switch (definition->features[index].state) {
        case UMI_EXPERIENCE_FEATURE_VERIFIED: total += 100U; break;
        case UMI_EXPERIENCE_FEATURE_IMPLEMENTED: total += 75U; break;
        case UMI_EXPERIENCE_FEATURE_FOUNDATION: total += 40U; break;
        case UMI_EXPERIENCE_FEATURE_PLANNED:
        default: break;
        }
    }
    return total / (unsigned)definition->feature_count;
}

const char *umi_experience_feature_state_text(UmiExperienceFeatureState state)
{
    switch (state) {
    case UMI_EXPERIENCE_FEATURE_PLANNED: return "planned";
    case UMI_EXPERIENCE_FEATURE_FOUNDATION: return "foundation";
    case UMI_EXPERIENCE_FEATURE_IMPLEMENTED: return "implemented";
    case UMI_EXPERIENCE_FEATURE_VERIFIED: return "verified";
    default: return "unknown";
    }
}

const char *umi_experience_priority_text(UmiExperiencePriority priority)
{
    switch (priority) {
    case UMI_EXPERIENCE_PRIORITY_P0: return "P0";
    case UMI_EXPERIENCE_PRIORITY_P1: return "P1";
    case UMI_EXPERIENCE_PRIORITY_P2: return "P2";
    case UMI_EXPERIENCE_PRIORITY_P3: return "P3";
    case UMI_EXPERIENCE_PRIORITY_P4: return "P4";
    default: return "unknown";
    }
}

const char *umi_experience_ownership_text(UmiExperienceOwnership owner)
{
    switch (owner) {
    case UMI_EXPERIENCE_OWNER_FRAMEWORK: return "framework";
    case UMI_EXPERIENCE_OWNER_APPLICATION: return "application";
    case UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER: return "external-adapter";
    default: return "unknown";
    }
}
