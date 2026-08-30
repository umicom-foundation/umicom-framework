/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/command_binding.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/command_binding.h"

#include <string.h>

UmiStatus umi_application_production_command_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCommandBindings *out_bindings)
{
    size_t index;
    UmiStatus status;
    if (out_bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_bindings, 0, sizeof(*out_bindings));
    status = umi_application_command_surface_build(
        binding->experience, &out_bindings->surface);
    if (status != UMI_STATUS_OK) return status;
    if (out_bindings->surface.command_count >
        UMI_APPLICATION_PRODUCTION_MAX_COMMANDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < out_bindings->surface.command_count; ++index) {
        UmiApplicationProductionCommandBinding *entry =
            &out_bindings->entries[index];
        entry->command = &out_bindings->surface.commands[index];
        entry->enabled =
            entry->command->kind != UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE ||
            entry->command->feature_state != UMI_EXPERIENCE_FEATURE_PLANNED;
        entry->mutates_workspace =
            entry->command->kind != UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE;
        out_bindings->enabled_count += (size_t)entry->enabled;
        out_bindings->count += 1U;
    }
    return UMI_STATUS_OK;
}

const UmiApplicationProductionCommandBinding *
umi_application_production_command_bindings_find(
    const UmiApplicationProductionCommandBindings *bindings,
    UmiApplicationCommandKind kind, const char *target_id)
{
    size_t index;
    if (bindings == NULL || target_id == NULL) return NULL;
    for (index = 0U; index < bindings->count; ++index) {
        const UmiApplicationCommandDescriptor *command =
            bindings->entries[index].command;
        if (command != NULL && command->kind == kind &&
            strcmp(command->target_id, target_id) == 0)
            return &bindings->entries[index];
    }
    return NULL;
}

