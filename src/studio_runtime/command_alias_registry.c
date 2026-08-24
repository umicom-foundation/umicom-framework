/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_alias_registry.c
 *
 * PURPOSE:
 *   Implement non-destructive command alias registration and forwarding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_alias_registry.h"

#include <stdlib.h>
#include <string.h>

typedef struct AliasBinding {
    struct UmiStudioRuntimeCommandAliasRegistry *owner;
    const UmiStudioRuntimeCommandAliasDefinition *definition;
} AliasBinding;

struct UmiStudioRuntimeCommandAliasRegistry {
    UmiStudioRuntimeBindings *bindings;
    AliasBinding bindings_storage[64];
    size_t registered_count;
};

static int alias_enabled(void *user_data, const char *argument)
{
    AliasBinding *binding = (AliasBinding *)user_data;
    const UmiStudioRuntimeCommandAliasDefinition *definition;

    if (binding == NULL || binding->owner == NULL ||
        binding->definition == NULL) {
        return 0;
    }

    definition = binding->definition;

    if (definition->target_command_id != NULL &&
        definition->target_command_id[0] != '\0') {
        return umi_command_registry_is_enabled(
            binding->owner->bindings->commands,
            definition->target_command_id,
            argument);
    }

    if (definition->activate_surface) {
        return umi_studio_surface_catalogue_for_kind(
            definition->surface) != NULL;
    }

    return 1;
}

static UmiStatus alias_execute(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    AliasBinding *binding = (AliasBinding *)user_data;
    const UmiStudioRuntimeCommandAliasDefinition *definition;
    UmiStatus status = UMI_STATUS_OK;

    if (binding == NULL || binding->owner == NULL ||
        binding->definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    definition = binding->definition;

    if (definition->target_command_id != NULL &&
        definition->target_command_id[0] != '\0') {
        status = umi_command_registry_execute(
            binding->owner->bindings->commands,
            definition->target_command_id,
            argument,
            out_message,
            message_capacity);
        if (status != UMI_STATUS_OK) return status;
    }

    if (definition->activate_surface) {
        status = umi_studio_surface_activate_kind(
            binding->owner->bindings,
            definition->surface,
            1,
            1);
    }

    return status;
}

UmiStatus umi_studio_command_alias_registry_create(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeCommandAliasRegistry **out_registry)
{
    UmiStudioRuntimeCommandAliasRegistry *registry;
    UmiStatus status;

    if (bindings == NULL || out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    *out_registry = NULL;

    registry = (UmiStudioRuntimeCommandAliasRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->bindings = bindings;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_studio_command_alias_registry_destroy(
    UmiStudioRuntimeCommandAliasRegistry *registry)
{
    free(registry);
}

UmiStatus umi_studio_command_alias_registry_install(
    UmiStudioRuntimeCommandAliasRegistry *registry)
{
    size_t index;

    if (registry == NULL || registry->bindings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_studio_command_alias_count() >
        sizeof(registry->bindings_storage) /
            sizeof(registry->bindings_storage[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < umi_studio_command_alias_count(); ++index) {
        const UmiStudioRuntimeCommandAliasDefinition *source =
            umi_studio_command_alias_at(index);
        UmiCommandDescriptor descriptor;
        UmiStatus status;

        if (source == NULL) return UMI_STATUS_INTERNAL_ERROR;

        status = umi_studio_command_alias_validate(source);
        if (status != UMI_STATUS_OK) return status;

        /*
         * Existing operational commands always take precedence. This is the
         * key non-destructive compatibility rule for legacy shell command IDs.
         */
        if (umi_command_registry_contains(
                registry->bindings->commands,
                source->alias_id)) {
            continue;
        }

        registry->bindings_storage[index].owner = registry;
        registry->bindings_storage[index].definition = source;

        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.command_id = source->alias_id;
        descriptor.title = source->title;
        descriptor.category = source->category;
        descriptor.description = source->description;
        descriptor.required_permission =
            source->mutates_state
                ? "studio.control"
                : "studio.read";
        descriptor.flags =
            source->mutates_state
                ? UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED
                : UMI_COMMAND_NONE;
        descriptor.handler = alias_execute;
        descriptor.enabled = alias_enabled;
        descriptor.user_data = &registry->bindings_storage[index];

        status = umi_command_registry_register(
            registry->bindings->commands,
            &descriptor);
        if (status != UMI_STATUS_OK) return status;

        registry->registered_count += 1U;
    }

    return UMI_STATUS_OK;
}

size_t umi_studio_command_alias_registry_registered_count(
    const UmiStudioRuntimeCommandAliasRegistry *registry)
{
    return registry != NULL ? registry->registered_count : 0U;
}
