/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_alias_registry.c
 *
 * PURPOSE:
 *   Implement non-destructive command alias registration and forwarding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the alias enabled operation used by this module and its client applications. */
static int alias_enabled(void *user_data, const char *argument)
{
    AliasBinding *binding = (AliasBinding *)user_data;
    const UmiStudioRuntimeCommandAliasDefinition *definition;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->owner == NULL ||
        binding->definition == NULL) {
        return 0;
    }

    definition = binding->definition;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition->target_command_id != NULL &&
        definition->target_command_id[0] != '\0') {
        return umi_command_registry_is_enabled(
            binding->owner->bindings->commands,
            definition->target_command_id,
            argument);
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->activate_surface) {
        return umi_studio_surface_catalogue_for_kind(
            definition->surface) != NULL;
    }

    return 1;
}

/*
 * Perform alias through the module contract so client applications do not duplicate its
 * policy.
 */
static UmiStatus alias_execute(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    AliasBinding *binding = (AliasBinding *)user_data;
    const UmiStudioRuntimeCommandAliasDefinition *definition;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->owner == NULL ||
        binding->definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    definition = binding->definition;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition->target_command_id != NULL &&
        definition->target_command_id[0] != '\0') {
        status = umi_command_registry_execute(
            binding->owner->bindings->commands,
            definition->target_command_id,
            argument,
            out_message,
            message_capacity);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->activate_surface) {
        status = umi_studio_surface_activate_kind(
            binding->owner->bindings,
            definition->surface,
            1,
            1);
    }

    return status;
}

/*
 * Initialise studio command alias registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_command_alias_registry_create(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeCommandAliasRegistry **out_registry)
{
    UmiStudioRuntimeCommandAliasRegistry *registry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    *out_registry = NULL;

    registry = (UmiStudioRuntimeCommandAliasRegistry *)calloc(
        1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->bindings = bindings;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by studio command alias registry so the same storage can be
 * reused safely.
 */
void umi_studio_command_alias_registry_destroy(
    UmiStudioRuntimeCommandAliasRegistry *registry)
{
    free(registry);
}

/*
 * Provide the studio command alias registry install operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_command_alias_registry_install(
    UmiStudioRuntimeCommandAliasRegistry *registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || registry->bindings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_studio_command_alias_count() >
        sizeof(registry->bindings_storage) /
            sizeof(registry->bindings_storage[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_studio_command_alias_count(); ++index) {
        const UmiStudioRuntimeCommandAliasDefinition *source =
            umi_studio_command_alias_at(index);
        UmiCommandDescriptor descriptor;
        UmiStatus status;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (source == NULL) return UMI_STATUS_INTERNAL_ERROR;

        status = umi_studio_command_alias_validate(source);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        registry->registered_count += 1U;
    }

    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by studio command alias registry registered
 * without changing their state.
 */
size_t umi_studio_command_alias_registry_registered_count(
    const UmiStudioRuntimeCommandAliasRegistry *registry)
{
    return registry != NULL ? registry->registered_count : 0U;
}
