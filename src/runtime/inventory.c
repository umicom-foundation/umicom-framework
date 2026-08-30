/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/inventory.c
 *
 * PURPOSE:
 *   Implement registry inventory snapshots and namespace-prefix queries used
 *   by applications, diagnostics, compatibility checks and regression tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/inventory.h"

#include <string.h>

static int text_is_present(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static int text_has_prefix(const char *text, const char *prefix)
{
    size_t prefix_length;

    if (text == NULL || prefix == NULL) {
        return 0;
    }
    prefix_length = strlen(prefix);
    return strncmp(text, prefix, prefix_length) == 0;
}

UmiStatus umi_runtime_inventory_snapshot(
    const UmiCommandRegistry *commands,
    const UmiServiceRegistry *services,
    const UmiCapabilityRegistry *capabilities,
    UmiRuntimeInventorySnapshot *out_snapshot)
{
    size_t index;

    if (out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_RUNTIME_INVENTORY_API_VERSION;

    if (commands != NULL) {
        out_snapshot->command_count = umi_command_registry_count(commands);
        for (index = 0U; index < out_snapshot->command_count; ++index) {
            UmiCommandSnapshot command;
            UmiStatus status = umi_command_registry_at(commands, index, &command);
            if (status != UMI_STATUS_OK) {
                return status;
            }
            if ((command.flags & UMI_COMMAND_MUTATES_STATE) != 0U) {
                ++out_snapshot->mutating_command_count;
            }
            if ((command.flags & UMI_COMMAND_BACKGROUND) != 0U) {
                ++out_snapshot->background_command_count;
            }
            if ((command.flags & UMI_COMMAND_AUDITED) != 0U) {
                ++out_snapshot->audited_command_count;
            }
            if ((command.flags & UMI_COMMAND_REQUIRES_TRUST) != 0U) {
                ++out_snapshot->trusted_command_count;
            }
        }
    }

    if (services != NULL) {
        out_snapshot->service_count = umi_service_registry_count(services);
        for (index = 0U; index < out_snapshot->service_count; ++index) {
            const UmiServiceDescriptor *service = umi_service_registry_at(services, index);
            if (service == NULL) {
                return UMI_STATUS_INTERNAL_ERROR;
            }
            if ((service->flags & UMI_SERVICE_SINGLETON) != 0U) {
                ++out_snapshot->singleton_service_count;
            }
            if ((service->flags & UMI_SERVICE_OWNED) != 0U) {
                ++out_snapshot->owned_service_count;
            }
            if ((service->flags & UMI_SERVICE_THREAD_SAFE) != 0U) {
                ++out_snapshot->thread_safe_service_count;
            }
            if ((service->flags & UMI_SERVICE_REPLACEABLE) != 0U) {
                ++out_snapshot->replaceable_service_count;
            }
        }
    }

    if (capabilities != NULL) {
        out_snapshot->capability_count =
            umi_capability_registry_count(capabilities);
        for (index = 0U; index < out_snapshot->capability_count; ++index) {
            const UmiCapabilityDescriptor *capability =
                umi_capability_registry_at(capabilities, index);
            if (capability == NULL) {
                return UMI_STATUS_INTERNAL_ERROR;
            }
            if ((capability->flags & UMI_CAPABILITY_SINGLETON) != 0U) {
                ++out_snapshot->singleton_capability_count;
            }
            if ((capability->flags & UMI_CAPABILITY_EXTERNAL) != 0U) {
                ++out_snapshot->external_capability_count;
            }
            if ((capability->flags & UMI_CAPABILITY_RELOAD_SAFE) != 0U) {
                ++out_snapshot->reload_safe_capability_count;
            }
            if ((capability->flags & UMI_CAPABILITY_OPTIONAL) != 0U) {
                ++out_snapshot->optional_capability_count;
            }
        }
    }

    return UMI_STATUS_OK;
}

int umi_runtime_inventory_has_command(const UmiCommandRegistry *registry,
                                      const char *command_id)
{
    return registry != NULL && text_is_present(command_id)
        ? umi_command_registry_contains(registry, command_id)
        : 0;
}

int umi_runtime_inventory_has_service(const UmiServiceRegistry *registry,
                                      const char *service_id)
{
    return registry != NULL && text_is_present(service_id) &&
           umi_service_registry_find(registry, service_id) != NULL;
}

int umi_runtime_inventory_has_capability(
    const UmiCapabilityRegistry *registry,
    const char *capability_id)
{
    return registry != NULL && text_is_present(capability_id) &&
           umi_capability_registry_find(registry, capability_id) != NULL;
}

UmiStatus umi_runtime_inventory_count_command_prefix(
    const UmiCommandRegistry *registry,
    const char *prefix,
    size_t *out_count)
{
    size_t index;
    size_t count = 0U;

    if (registry == NULL || prefix == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_command_registry_count(registry); ++index) {
        UmiCommandSnapshot command;
        UmiStatus status = umi_command_registry_at(registry, index, &command);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        if (text_has_prefix(command.command_id, prefix)) {
            ++count;
        }
    }
    *out_count = count;
    return UMI_STATUS_OK;
}

UmiStatus umi_runtime_inventory_count_service_prefix(
    const UmiServiceRegistry *registry,
    const char *prefix,
    size_t *out_count)
{
    size_t index;
    size_t count = 0U;

    if (registry == NULL || prefix == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_service_registry_count(registry); ++index) {
        const UmiServiceDescriptor *service = umi_service_registry_at(registry, index);
        if (service == NULL) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
        if (text_has_prefix(service->service_id, prefix)) {
            ++count;
        }
    }
    *out_count = count;
    return UMI_STATUS_OK;
}

UmiStatus umi_runtime_inventory_count_capability_prefix(
    const UmiCapabilityRegistry *registry,
    const char *prefix,
    size_t *out_count)
{
    size_t index;
    size_t count = 0U;

    if (registry == NULL || prefix == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_capability_registry_count(registry); ++index) {
        const UmiCapabilityDescriptor *capability =
            umi_capability_registry_at(registry, index);
        if (capability == NULL) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
        if (text_has_prefix(capability->capability_id, prefix)) {
            ++count;
        }
    }
    *out_count = count;
    return UMI_STATUS_OK;
}
