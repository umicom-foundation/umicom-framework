/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/command_registry_bridge.c
 *
 * PURPOSE:
 *   Register and execute AI Developer commands through UmiCommandRegistry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/command_registry_bridge.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperCommandRegistryBridge {
    UmiCommandRegistry *registry;
    UmiAiDeveloperExperiencePlatform *platform;
    UmiAiDeveloperCommandContext context;
};

typedef struct CommandBinding {
    UmiAiDeveloperCommandRegistryBridge *bridge;
    const char *command_id;
} CommandBinding;

typedef struct BridgeStorage {
    UmiAiDeveloperCommandRegistryBridge base;
    CommandBinding bindings[32];
} BridgeStorage;

static UmiStatus binding_execute(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    CommandBinding *binding = (CommandBinding *)user_data;
    UmiAiDeveloperCommandContext context;

    if (binding == NULL || binding->bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    context = binding->bridge->context;

    /*
     * Optional command argument is used as an approval or patch identifier for
     * host integrations that can only pass one string.
     */
    if (argument != NULL && argument[0] != '\0') {
        const size_t length = strlen(argument);

        if (length < sizeof(context.approval_id)) {
            (void)memcpy(context.approval_id, argument, length + 1U);
        }
    }

    return umi_ai_developer_command_execute(
        binding->bridge->platform,
        binding->command_id,
        &context,
        out_message,
        message_capacity);
}

static int binding_enabled(
    void *user_data,
    const char *argument)
{
    CommandBinding *binding = (CommandBinding *)user_data;
    UmiAiDeveloperCommandContext context;

    if (binding == NULL || binding->bridge == NULL) return 0;

    context = binding->bridge->context;

    if (argument != NULL && argument[0] != '\0' &&
        strlen(argument) < sizeof(context.approval_id)) {
        (void)strcpy(context.approval_id, argument);
    }

    return umi_ai_developer_command_enabled(
        binding->bridge->platform,
        binding->command_id,
        &context);
}

UmiStatus umi_ai_developer_command_registry_bridge_create(
    UmiCommandRegistry *registry,
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperCommandRegistryBridge **out_bridge)
{
    BridgeStorage *storage;

    if (registry == NULL || platform == NULL || out_bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bridge = NULL;

    storage = (BridgeStorage *)calloc(1U, sizeof(*storage));
    if (storage == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    storage->base.registry = registry;
    storage->base.platform = platform;
    umi_ai_developer_command_context_init(&storage->base.context);

    *out_bridge = &storage->base;
    return UMI_STATUS_OK;
}

void umi_ai_developer_command_registry_bridge_destroy(
    UmiAiDeveloperCommandRegistryBridge *bridge)
{
    free(bridge);
}

UmiStatus umi_ai_developer_command_registry_bridge_set_context(
    UmiAiDeveloperCommandRegistryBridge *bridge,
    const UmiAiDeveloperCommandContext *context)
{
    if (bridge == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bridge->context = *context;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_command_registry_bridge_register(
    UmiAiDeveloperCommandRegistryBridge *bridge)
{
    BridgeStorage *storage = (BridgeStorage *)bridge;
    size_t count;
    size_t index;

    if (bridge == NULL || bridge->registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    count = umi_ai_developer_command_count();
    if (count > sizeof(storage->bindings) / sizeof(storage->bindings[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < count; ++index) {
        const UmiAiDeveloperCommandDescriptor *source =
            umi_ai_developer_command_at(index);
        UmiCommandDescriptor descriptor;
        UmiStatus status;

        if (source == NULL) return UMI_STATUS_INTERNAL_ERROR;

        if (umi_command_registry_contains(
                bridge->registry,
                source->command_id)) {
            continue;
        }

        storage->bindings[index].bridge = bridge;
        storage->bindings[index].command_id = source->command_id;

        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.command_id = source->command_id;
        descriptor.title = source->label;
        descriptor.category = "AI Developer";
        descriptor.description = source->description;
        descriptor.required_permission =
            source->mutates_state
                ? "ai.developer.control"
                : "ai.developer.read";
        descriptor.flags =
            source->mutates_state
                ? UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED
                : UMI_COMMAND_NONE;
        descriptor.handler = binding_execute;
        descriptor.enabled = binding_enabled;
        descriptor.user_data = &storage->bindings[index];

        status = umi_command_registry_register(
            bridge->registry,
            &descriptor);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
