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
    memset(out_surface, 0, sizeof(*out_surface));
    out_surface->structure_size = sizeof(*out_surface);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->panel_count; ++index) {
        result = append_command(out_surface, UMI_APPLICATION_COMMAND_ACTIVATE_PANEL,
            experience->panels[index].panel_id, experience->panels[index].title,
            UMI_EXPERIENCE_FEATURE_IMPLEMENTED);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->layout_count; ++index) {
        result = append_command(out_surface, UMI_APPLICATION_COMMAND_SELECT_LAYOUT,
            experience->layouts[index].layout_id, experience->layouts[index].title,
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
    if (surface == NULL || target_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < surface->command_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (surface->commands[index].kind == kind &&
            strcmp(surface->commands[index].target_id, target_id) == 0)
            return &surface->commands[index];
    }
    return NULL;
}
