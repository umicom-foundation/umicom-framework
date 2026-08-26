/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/command_surface.c
 *
 * PURPOSE:
 *   Build bounded command descriptors from panels, layouts and implemented product features.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/command_surface.h"

#include <string.h>

static UmiStatus append_command(
    UmiApplicationCommandSurface *surface,
    UmiApplicationCommandKind kind,
    const char *target_id,
    const char *title,
    UmiExperienceFeatureState state)
{
    UmiApplicationCommandDescriptor *command;
    if (surface->command_count >= UMI_APPLICATION_RUNTIME_MAX_COMMANDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    command = &surface->commands[surface->command_count++];
    command->kind = kind;
    command->target_id = target_id;
    command->title = title;
    command->feature_state = state;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_command_surface_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCommandSurface *out_surface)
{
    size_t index;
    UmiStatus result;
    if (experience == NULL || out_surface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_surface, 0, sizeof(*out_surface));
    out_surface->structure_size = sizeof(*out_surface);
    for (index = 0U; index < experience->panel_count; ++index) {
        result = append_command(out_surface, UMI_APPLICATION_COMMAND_ACTIVATE_PANEL,
            experience->panels[index].panel_id, experience->panels[index].title,
            UMI_EXPERIENCE_FEATURE_IMPLEMENTED);
        if (result != UMI_STATUS_OK) return result;
    }
    for (index = 0U; index < experience->layout_count; ++index) {
        result = append_command(out_surface, UMI_APPLICATION_COMMAND_SELECT_LAYOUT,
            experience->layouts[index].layout_id, experience->layouts[index].title,
            UMI_EXPERIENCE_FEATURE_IMPLEMENTED);
        if (result != UMI_STATUS_OK) return result;
    }
    for (index = 0U; index < experience->feature_count; ++index) {
        if (experience->features[index].state >= UMI_EXPERIENCE_FEATURE_IMPLEMENTED) {
            result = append_command(out_surface, UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE,
                experience->features[index].feature_id, experience->features[index].title,
                experience->features[index].state);
            if (result != UMI_STATUS_OK) return result;
        }
    }
    return UMI_STATUS_OK;
}

const UmiApplicationCommandDescriptor *umi_application_command_surface_find(
    const UmiApplicationCommandSurface *surface,
    UmiApplicationCommandKind kind,
    const char *target_id)
{
    size_t index;
    if (surface == NULL || target_id == NULL) return NULL;
    for (index = 0U; index < surface->command_count; ++index) {
        if (surface->commands[index].kind == kind &&
            strcmp(surface->commands[index].target_id, target_id) == 0)
            return &surface->commands[index];
    }
    return NULL;
}
