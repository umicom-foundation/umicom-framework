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

/* Provide the text present operation used by this module and its client applications. */
static int text_present(const char *text)
{
    return text != NULL && text[0] != '\0';
}

/*
 * Provide the valid feature state operation used by this module and its client
 * applications.
 */
static int valid_feature_state(UmiExperienceFeatureState state)
{
    return state >= UMI_EXPERIENCE_FEATURE_PLANNED &&
           state <= UMI_EXPERIENCE_FEATURE_VERIFIED;
}

/* Provide the valid priority operation used by this module and its client applications. */
static int valid_priority(UmiExperiencePriority priority)
{
    return priority >= UMI_EXPERIENCE_PRIORITY_P0 &&
           priority <= UMI_EXPERIENCE_PRIORITY_P4;
}

/* Provide the valid owner operation used by this module and its client applications. */
static int valid_owner(UmiExperienceOwnership owner)
{
    return owner >= UMI_EXPERIENCE_OWNER_FRAMEWORK &&
           owner <= UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER;
}

/*
 * Find application experience panel while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiExperiencePanelDefinition *umi_application_experience_panel_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || !text_present(panel_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->panel_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(definition->panels[index].panel_id, panel_id) == 0)
            return &definition->panels[index];
    }
    return NULL;
}

/*
 * Find application experience layout while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiExperienceLayoutDefinition *umi_application_experience_layout_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *layout_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || !text_present(layout_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->layout_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(definition->layouts[index].layout_id, layout_id) == 0)
            return &definition->layouts[index];
    }
    return NULL;
}

/*
 * Find application experience feature while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiExperienceFeatureDefinition *umi_application_experience_feature_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *feature_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || !text_present(feature_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->feature_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(definition->features[index].feature_id, feature_id) == 0)
            return &definition->features[index];
    }
    return NULL;
}

/*
 * Check that application experience satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_application_experience_validate(
    const UmiApplicationExperienceDefinition *definition)
{
    size_t index;
    size_t nested;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel = &definition->panels[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (panel->structure_size != sizeof(*panel) ||
            !text_present(panel->panel_id) ||
            !text_present(panel->title) ||
            !text_present(panel->summary) ||
            !text_present(panel->default_region))
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Visit each bounded item once so every record receives the same rule. */
        for (nested = index + 1U; nested < definition->panel_count; ++nested) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(panel->panel_id,
                       definition->panels[nested].panel_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout = &definition->layouts[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (layout->structure_size != sizeof(*layout) ||
            !text_present(layout->layout_id) ||
            !text_present(layout->title) ||
            !text_present(layout->description) ||
            layout->panel_ids == NULL || layout->panel_count == 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Visit each bounded item once so every record receives the same rule. */
        for (nested = 0U; nested < layout->panel_count; ++nested) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_application_experience_panel_find(
                    definition, layout->panel_ids[nested]) == NULL)
                return UMI_STATUS_NOT_FOUND;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (nested = index + 1U; nested < definition->layout_count; ++nested) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(layout->layout_id,
                       definition->layouts[nested].layout_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_experience_layout_find(
            definition, definition->default_layout_id) == NULL)
        return UMI_STATUS_NOT_FOUND;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &definition->features[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (feature->structure_size != sizeof(*feature) ||
            !text_present(feature->feature_id) ||
            !text_present(feature->title) ||
            !text_present(feature->summary) ||
            !valid_feature_state(feature->state) ||
            !valid_priority(feature->priority) ||
            !valid_owner(feature->owner))
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Visit each bounded item once so every record receives the same rule. */
        for (nested = index + 1U; nested < definition->feature_count; ++nested) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(feature->feature_id,
                       definition->features[nested].feature_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by application experience feature state without
 * changing their state.
 */
size_t umi_application_experience_feature_state_count(
    const UmiApplicationExperienceDefinition *definition,
    UmiExperienceFeatureState state)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || !valid_feature_state(state)) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->feature_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (definition->features[index].state == state) ++count;
    }
    return count;
}

/*
 * Provide the application experience readiness percent operation used by this module and
 * its client applications.
 */
unsigned umi_application_experience_readiness_percent(
    const UmiApplicationExperienceDefinition *definition)
{
    size_t index;
    unsigned total = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || definition->feature_count == 0U) return 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->feature_count; ++index) {
        /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the experience feature state text operation used by this module and its client
 * applications.
 */
const char *umi_experience_feature_state_text(UmiExperienceFeatureState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_EXPERIENCE_FEATURE_PLANNED: return "planned";
    case UMI_EXPERIENCE_FEATURE_FOUNDATION: return "foundation";
    case UMI_EXPERIENCE_FEATURE_IMPLEMENTED: return "implemented";
    case UMI_EXPERIENCE_FEATURE_VERIFIED: return "verified";
    default: return "unknown";
    }
}

/*
 * Provide the experience priority text operation used by this module and its client
 * applications.
 */
const char *umi_experience_priority_text(UmiExperiencePriority priority)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (priority) {
    case UMI_EXPERIENCE_PRIORITY_P0: return "P0";
    case UMI_EXPERIENCE_PRIORITY_P1: return "P1";
    case UMI_EXPERIENCE_PRIORITY_P2: return "P2";
    case UMI_EXPERIENCE_PRIORITY_P3: return "P3";
    case UMI_EXPERIENCE_PRIORITY_P4: return "P4";
    default: return "unknown";
    }
}

/*
 * Provide the experience ownership text operation used by this module and its client
 * applications.
 */
const char *umi_experience_ownership_text(UmiExperienceOwnership owner)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (owner) {
    case UMI_EXPERIENCE_OWNER_FRAMEWORK: return "framework";
    case UMI_EXPERIENCE_OWNER_APPLICATION: return "application";
    case UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER: return "external-adapter";
    default: return "unknown";
    }
}
