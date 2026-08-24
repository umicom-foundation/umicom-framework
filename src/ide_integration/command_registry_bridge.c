/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/command_registry_bridge.c
 *
 * PURPOSE:
 *   Implement authoritative runtime command registration for IDE integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/command_registry_bridge.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiIdeCommandBinding {
    struct UmiIdeCommandRegistryBridge *owner;
    const char *command_id;
} UmiIdeCommandBinding;

struct UmiIdeCommandRegistryBridge {
    UmiCommandRegistry *registry;
    UmiIdeCommandRouter router;
    UmiIdeCommandBinding bindings[32];
    size_t binding_count;
};

static UmiStatus execute_binding(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiIdeCommandBinding *binding =
        (UmiIdeCommandBinding *)user_data;
    UmiIdeCommandContext context;

    if (binding == NULL || binding->owner == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    context = binding->owner->router.context;

    if (argument != NULL && argument[0] != '\0') {
        const size_t length = strlen(argument);

        if (length >= sizeof(context.argument)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(context.argument, argument, length + 1U);
    }

    (void)umi_ide_command_router_set_context(
        &binding->owner->router,
        &context);

    return umi_ide_command_router_execute(
        &binding->owner->router,
        binding->command_id,
        out_message,
        message_capacity);
}

static int enabled_binding(void *user_data, const char *argument)
{
    UmiIdeCommandBinding *binding =
        (UmiIdeCommandBinding *)user_data;

    (void)argument;

    return binding != NULL &&
        binding->owner != NULL &&
        umi_ide_command_router_enabled(
            &binding->owner->router,
            binding->command_id);
}

UmiStatus umi_ide_command_registry_bridge_create(
    UmiCommandRegistry *registry,
    UmiIdeIntegrationPlatform *platform,
    UmiIdeCommandRegistryBridge **out_bridge)
{
    UmiIdeCommandRegistryBridge *bridge;
    UmiStatus status;

    if (registry == NULL || platform == NULL || out_bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bridge = NULL;

    bridge = (UmiIdeCommandRegistryBridge *)calloc(1U, sizeof(*bridge));
    if (bridge == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    bridge->registry = registry;

    status = umi_ide_command_router_init(
        &bridge->router,
        platform);
    if (status != UMI_STATUS_OK) {
        free(bridge);
        return status;
    }

    *out_bridge = bridge;
    return UMI_STATUS_OK;
}

void umi_ide_command_registry_bridge_destroy(
    UmiIdeCommandRegistryBridge *bridge)
{
    free(bridge);
}

UmiStatus umi_ide_command_registry_bridge_set_context(
    UmiIdeCommandRegistryBridge *bridge,
    const UmiIdeCommandContext *context)
{
    if (bridge == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ide_command_router_set_context(
        &bridge->router,
        context);
}

UmiStatus umi_ide_command_registry_bridge_register(
    UmiIdeCommandRegistryBridge *bridge)
{
    size_t index;
    size_t count;

    if (bridge == NULL || bridge->registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    count = umi_ide_command_count();

    if (count > sizeof(bridge->bindings) / sizeof(bridge->bindings[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < count; ++index) {
        const UmiIdeCommandDescriptor *source = umi_ide_command_at(index);
        UmiCommandDescriptor descriptor;
        UmiStatus status;

        if (source == NULL) return UMI_STATUS_INTERNAL_ERROR;

        if (umi_command_registry_contains(
                bridge->registry,
                source->command_id)) {
            continue;
        }

        bridge->bindings[index].owner = bridge;
        bridge->bindings[index].command_id = source->command_id;

        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.command_id = source->command_id;
        descriptor.title = source->label;
        descriptor.category = source->category;
        descriptor.description = source->description;
        descriptor.required_permission =
            source->mutates_state
                ? "developer.ide.control"
                : "developer.ide.read";
        descriptor.flags =
            source->mutates_state
                ? UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED
                : UMI_COMMAND_NONE;
        descriptor.handler = execute_binding;
        descriptor.enabled = enabled_binding;
        descriptor.user_data = &bridge->bindings[index];

        status = umi_command_registry_register(
            bridge->registry,
            &descriptor);
        if (status != UMI_STATUS_OK) return status;

        bridge->binding_count += 1U;
    }

    return UMI_STATUS_OK;
}
