/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/action_registry.c
 *
 * PURPOSE:
 *   Implement bounded product-action bindings for reusable workbench commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/action_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchActionRegistry {
    UmiDeveloperWorkbenchActionBinding
        items[UMI_DEVELOPER_WORKBENCH_MAX_COMMANDS];
    size_t count;
    uint64_t revision;
};

static size_t find_index(
    const UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id)
{
    size_t index;

    if (registry == NULL || command_id == NULL) return (size_t)-1;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].command_id, command_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

UmiStatus umi_developer_workbench_action_registry_create(
    UmiDeveloperWorkbenchActionRegistry **out_registry)
{
    UmiDeveloperWorkbenchActionRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperWorkbenchActionRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_workbench_action_registry_destroy(
    UmiDeveloperWorkbenchActionRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_workbench_action_registry_bind(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    UmiDeveloperWorkbenchActionHandler handler,
    UmiDeveloperWorkbenchActionEnabled enabled,
    void *user_data)
{
    size_t index;
    size_t length;

    if (registry == NULL || command_id == NULL ||
        command_id[0] == '\0' || handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(command_id);
    if (length >= sizeof(registry->items[0].command_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    index = find_index(registry, command_id);
    if (index == (size_t)-1) {
        if (registry->count >= UMI_DEVELOPER_WORKBENCH_MAX_COMMANDS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }

    (void)memset(&registry->items[index], 0, sizeof(registry->items[index]));
    (void)memcpy(registry->items[index].command_id,
                 command_id,
                 length + 1U);
    registry->items[index].handler = handler;
    registry->items[index].enabled = enabled;
    registry->items[index].user_data = user_data;
    registry->items[index].revision = ++registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_action_registry_unbind(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id)
{
    size_t index;
    size_t tail;

    if (registry == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, command_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    tail = registry->count - index - 1U;
    if (tail > 0U) {
        (void)memmove(&registry->items[index],
                      &registry->items[index + 1U],
                      tail * sizeof(registry->items[0]));
    }

    registry->count -= 1U;
    (void)memset(&registry->items[registry->count],
                 0,
                 sizeof(registry->items[0]));
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_action_registry_find(
    const UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    UmiDeveloperWorkbenchActionBinding *out_binding)
{
    size_t index;

    if (registry == NULL || command_id == NULL || out_binding == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, command_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_binding = registry->items[index];
    return UMI_STATUS_OK;
}

int umi_developer_workbench_action_registry_is_enabled(
    const UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    const char *argument)
{
    UmiDeveloperWorkbenchActionBinding binding;

    if (umi_developer_workbench_action_registry_find(
            registry, command_id, &binding) != UMI_STATUS_OK) {
        return 0;
    }

    return binding.enabled == NULL ||
        binding.enabled(binding.user_data, argument);
}

UmiStatus umi_developer_workbench_action_registry_execute(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperWorkbenchActionBinding binding;
    UmiStatus status;

    status = umi_developer_workbench_action_registry_find(
        registry, command_id, &binding);
    if (status != UMI_STATUS_OK) return status;

    if (binding.enabled != NULL &&
        !binding.enabled(binding.user_data, argument)) {
        return UMI_STATUS_UNAVAILABLE;
    }

    return binding.handler(
        binding.user_data,
        argument,
        out_message,
        message_capacity);
}

size_t umi_developer_workbench_action_registry_count(
    const UmiDeveloperWorkbenchActionRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
