/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/command_registry_bridge.c
 *
 * PURPOSE:
 *   Implement authoritative runtime command registration for IDE integration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the execute binding operation used by this module and its client applications. */
static UmiStatus execute_binding(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiIdeCommandBinding *binding =
        (UmiIdeCommandBinding *)user_data;
    UmiIdeCommandContext context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->owner == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    context = binding->owner->router.context;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (argument != NULL && argument[0] != '\0') {
        const size_t length = strlen(argument);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the enabled binding operation used by this module and its client applications. */
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

/*
 * Initialise ide command registry bridge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ide_command_registry_bridge_create(
    UmiCommandRegistry *registry,
    UmiIdeIntegrationPlatform *platform,
    UmiIdeCommandRegistryBridge **out_bridge)
{
    UmiIdeCommandRegistryBridge *bridge;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || platform == NULL || out_bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bridge = NULL;

    bridge = (UmiIdeCommandRegistryBridge *)calloc(1U, sizeof(*bridge));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    bridge->registry = registry;

    status = umi_ide_command_router_init(
        &bridge->router,
        platform);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(bridge);
        return status;
    }

    *out_bridge = bridge;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ide command registry bridge so the same storage can be
 * reused safely.
 */
void umi_ide_command_registry_bridge_destroy(
    UmiIdeCommandRegistryBridge *bridge)
{
    free(bridge);
}

/*
 * Provide the ide command registry bridge set context operation used by this module and
 * its client applications.
 */
UmiStatus umi_ide_command_registry_bridge_set_context(
    UmiIdeCommandRegistryBridge *bridge,
    const UmiIdeCommandContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ide_command_router_set_context(
        &bridge->router,
        context);
}

/*
 * Add ide command registry bridge only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ide_command_registry_bridge_register(
    UmiIdeCommandRegistryBridge *bridge)
{
    size_t index;
    size_t count;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || bridge->registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    count = umi_ide_command_count();

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > sizeof(bridge->bindings) / sizeof(bridge->bindings[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const UmiIdeCommandDescriptor *source = umi_ide_command_at(index);
        UmiCommandDescriptor descriptor;
        UmiStatus status;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (source == NULL) return UMI_STATUS_INTERNAL_ERROR;

        /* Apply this branch only when its contract condition is satisfied. */
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
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        bridge->binding_count += 1U;
    }

    return UMI_STATUS_OK;
}
