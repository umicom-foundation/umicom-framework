/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/command_surface.c
 *
 * PURPOSE:
 *   Build bounded command descriptors from panels, layouts and implemented product features.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/command_surface.h"

#include <string.h>

/* Provide the append command operation used by this module and its client applications. */
static UmiStatus append_command(
    UmiApplicationCommandSurface *surface,
    UmiApplicationCommandKind kind,
    const char *target_id,
    const char *title,
    const char *required_capability,
    UmiExperienceFeatureState state)
{
    UmiApplicationCommandDescriptor *command;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (surface->command_count >= UMI_APPLICATION_RUNTIME_MAX_COMMANDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    command = &surface->commands[surface->command_count++];
    command->kind = kind;
    command->target_id = target_id;
    command->title = title;
    command->required_capability = required_capability;
    command->feature_state = state;
    return UMI_STATUS_OK;
}

/*
 * Provide the application command surface build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_command_surface_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCommandSurface *out_surface)
{
    size_t index;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_surface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate counts and nested pointers before turning borrowed metadata into commands. */
    result = umi_application_experience_validate(experience);
    if (result != UMI_STATUS_OK) return result;
    memset(out_surface, 0, sizeof(*out_surface));
    out_surface->structure_size = sizeof(*out_surface);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->panel_count; ++index) {
        result = append_command(out_surface, UMI_APPLICATION_COMMAND_ACTIVATE_PANEL,
            experience->panels[index].panel_id, experience->panels[index].title,
            experience->panels[index].required_capability,
            UMI_EXPERIENCE_FEATURE_IMPLEMENTED);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->layout_count; ++index) {
        result = append_command(out_surface, UMI_APPLICATION_COMMAND_SELECT_LAYOUT,
            experience->layouts[index].layout_id, experience->layouts[index].title,
            NULL,
            UMI_EXPERIENCE_FEATURE_IMPLEMENTED);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->feature_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (experience->features[index].state >= UMI_EXPERIENCE_FEATURE_IMPLEMENTED) {
            result = append_command(out_surface, UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE,
                experience->features[index].feature_id, experience->features[index].title,
                experience->features[index].required_capability,
                experience->features[index].state);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (result != UMI_STATUS_OK) return result;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Find application command surface while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationCommandDescriptor *umi_application_command_surface_find(
    const UmiApplicationCommandSurface *surface,
    UmiApplicationCommandKind kind,
    const char *target_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || target_id == NULL ||
        surface->command_count > UMI_APPLICATION_RUNTIME_MAX_COMMANDS)
        return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < surface->command_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (surface->commands[index].target_id != NULL &&
            surface->commands[index].kind == kind &&
            strcmp(surface->commands[index].target_id, target_id) == 0)
            return &surface->commands[index];
    }
    return NULL;
}

/* Compare ASCII command text without changing the immutable catalogue strings. */
static int command_text_matches(const char *value, const char *query)
{
    size_t offset;

    /* An empty query intentionally matches every command. */
    if (query == NULL || query[0] == '\0') return 1;
    if (value == NULL) return 0;
    /* Search each possible starting position for a case-insensitive substring. */
    for (; *value != '\0'; ++value) {
        for (offset = 0U;; ++offset) {
            unsigned char left = (unsigned char)value[offset];
            unsigned char right = (unsigned char)query[offset];
            /* ASCII folding keeps the search deterministic across locales. */
            if (left >= (unsigned char)'A' && left <= (unsigned char)'Z') {
                left = (unsigned char)(left + ((unsigned char)'a' - (unsigned char)'A'));
            }
            if (right >= (unsigned char)'A' && right <= (unsigned char)'Z') {
                right = (unsigned char)(right + ((unsigned char)'a' - (unsigned char)'A'));
            }
            if (right == '\0') return 1;
            if (left == '\0' || left != right) break;
        }
    }
    return 0;
}

/* Check command maturity and an optional capability probe before exposing a result. */
bool umi_application_command_descriptor_is_available(
    const UmiApplicationCommandDescriptor *command,
    UmiApplicationCapabilityProbe capability_probe,
    void *capability_probe_data)
{
    /* Planned and foundation features remain visible only as unavailable commands. */
    if (command == NULL || command->feature_state < UMI_EXPERIENCE_FEATURE_IMPLEMENTED) {
        return false;
    }
    /* A provider can make a capability decision without changing catalogue metadata. */
    if (command->required_capability != NULL &&
        command->required_capability[0] != '\0' &&
        capability_probe != NULL) {
        return capability_probe(command->required_capability,
                                capability_probe_data) != 0;
    }
    return true;
}

/* Search one immutable command surface for palette entries matching a user query. */
UmiStatus umi_application_command_surface_query(
    const UmiApplicationCommandSurface *surface,
    const UmiApplicationCommandQuery *query,
    UmiApplicationCommandQueryResult *out_result)
{
    size_t index;

    /* Validate all public structures before reading their bounded command arrays. */
    if (surface == NULL || query == NULL || out_result == NULL ||
        surface->structure_size != sizeof(*surface) ||
        query->structure_size != sizeof(*query) ||
        surface->command_count > UMI_APPLICATION_RUNTIME_MAX_COMMANDS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->structure_size = (uint32_t)sizeof(*out_result);
    /* Visit every command once so ordering remains the stable catalogue order. */
    for (index = 0U; index < surface->command_count; ++index) {
        const UmiApplicationCommandDescriptor *command =
            &surface->commands[index];
        const bool available =
            umi_application_command_descriptor_is_available(
                command, query->capability_probe, query->capability_probe_data);
        const bool matches = command_text_matches(command->title, query->text) ||
            command_text_matches(command->target_id, query->text);
        /* Count unavailable matches even when the caller chooses not to display them. */
        if (matches && !available) out_result->unavailable_count += 1U;
        if (!matches || (!available && !query->include_unavailable)) continue;
        /* The result array has the same fixed capacity as the source surface. */
        if (out_result->match_count >= UMI_APPLICATION_RUNTIME_MAX_COMMANDS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_result->command_indices[out_result->match_count] = index;
        out_result->available[out_result->match_count] = available;
        out_result->match_count += 1U;
        if (available) out_result->available_count += 1U;
    }
    return UMI_STATUS_OK;
}
