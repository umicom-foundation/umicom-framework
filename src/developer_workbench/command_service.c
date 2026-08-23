/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/command_service.c
 *
 * PURPOSE:
 *   Implement executable/disable-aware professional workbench commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/command_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/developer_workbench/command_catalogue.h"
#include "umicom/developer_workbench/readiness.h"

typedef struct CommandBinding {
    struct UmiDeveloperWorkbenchCommandService *service;
    const UmiDeveloperWorkbenchCommandDefinition *definition;
} CommandBinding;

struct UmiDeveloperWorkbenchCommandService {
    UmiCommandRegistry *commands;
    UmiDeveloperWorkbenchLifecycle *lifecycle;
    UmiDeveloperWorkbenchConfigurationRegistry *configurations;
    const UmiToolchainProfile *profile;
    UmiDeveloperWorkbenchActionRegistry *actions;
    const UmiWorkspaceTrustStore *trust_store;
    CommandBinding bindings[UMI_DEVELOPER_WORKBENCH_MAX_COMMANDS];
    size_t binding_count;
};

static int workspace_is_trusted(
    UmiDeveloperWorkbenchCommandService *service)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiWorkspaceTrustRecord trust;

    if (service == NULL ||
        service->trust_store == NULL ||
        service->configurations == NULL) {
        return 0;
    }

    if (umi_developer_workbench_configuration_registry_active(
            service->configurations,
            &configuration) != UMI_STATUS_OK) {
        return 0;
    }

    if (umi_workspace_trust_store_get(
            service->trust_store,
            configuration.source_root,
            &trust) != UMI_STATUS_OK) {
        return 0;
    }

    return trust.level == UMI_WORKSPACE_TRUSTED;
}

static int lifecycle_enabled(
    UmiDeveloperWorkbenchCommandService *service,
    UmiDeveloperWorkbenchCommandAction action)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiDeveloperWorkbenchReadiness readiness;

    if (service == NULL ||
        service->configurations == NULL ||
        service->profile == NULL) {
        return 0;
    }

    if (action == UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL) {
        return service->lifecycle != NULL &&
            umi_developer_workbench_lifecycle_is_running(
                service->lifecycle);
    }

    if (umi_developer_workbench_configuration_registry_active(
            service->configurations,
            &configuration) != UMI_STATUS_OK) {
        return 0;
    }

    if (umi_developer_workbench_readiness_evaluate(
            &configuration,
            service->profile,
            &readiness) != UMI_STATUS_OK) {
        return 0;
    }

    switch (action) {
        case UMI_DEVELOPER_WORKBENCH_ACTION_CONFIGURE:
            return readiness.can_configure;
        case UMI_DEVELOPER_WORKBENCH_ACTION_BUILD:
            return readiness.can_build;
        case UMI_DEVELOPER_WORKBENCH_ACTION_REBUILD:
        case UMI_DEVELOPER_WORKBENCH_ACTION_CLEAN:
            return readiness.can_build &&
                configuration.build_directory[0] != '\0';
        case UMI_DEVELOPER_WORKBENCH_ACTION_TEST:
            return readiness.can_test;
        case UMI_DEVELOPER_WORKBENCH_ACTION_RUN:
            return readiness.can_run;
        case UMI_DEVELOPER_WORKBENCH_ACTION_INSTALL:
            return readiness.can_install;
        case UMI_DEVELOPER_WORKBENCH_ACTION_PACKAGE:
            return readiness.can_package;
        case UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL:
            return 1;
        case UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL:
        default:
            return 0;
    }
}

static int binding_enabled(void *user_data, const char *argument)
{
    CommandBinding *binding = (CommandBinding *)user_data;
    UmiDeveloperWorkbenchCommandService *service;

    if (binding == NULL || binding->service == NULL ||
        binding->definition == NULL) {
        return 0;
    }

    service = binding->service;

    if ((binding->definition->flags &
         UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST) != 0U &&
        !workspace_is_trusted(service)) {
        return 0;
    }

    if (binding->definition->action ==
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL) {
        return umi_developer_workbench_action_registry_is_enabled(
            service->actions,
            binding->definition->command_id,
            argument);
    }

    return lifecycle_enabled(service, binding->definition->action);
}

static UmiStatus binding_execute(void *user_data,
                                 const char *argument,
                                 char *out_message,
                                 size_t message_capacity)
{
    CommandBinding *binding = (CommandBinding *)user_data;
    UmiDeveloperWorkbenchCommandService *service;

    if (binding == NULL || binding->service == NULL ||
        binding->definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    service = binding->service;

    if (!binding_enabled(binding, argument)) {
        if (out_message != NULL && message_capacity > 0U) {
            (void)snprintf(
                out_message,
                message_capacity,
                "%s",
                "Command is unavailable in the current context.");
        }
        return UMI_STATUS_UNAVAILABLE;
    }

    if (binding->definition->action ==
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL) {
        return umi_developer_workbench_action_registry_execute(
            service->actions,
            binding->definition->command_id,
            argument,
            out_message,
            message_capacity);
    }

    {
        UmiBuildReport report;
        UmiStatus status;

        umi_build_report_init(&report);
        status = umi_developer_workbench_lifecycle_execute(
                service->lifecycle,
                binding->definition->command_id,
                binding->definition->action,
                &report);

        if (out_message != NULL && message_capacity > 0U) {
            if (report.last_output[0] != '\0') {
                (void)snprintf(out_message,
                               message_capacity,
                               "%s",
                               report.last_output);
            } else {
                (void)snprintf(
                    out_message,
                    message_capacity,
                    "%s: %s",
                    binding->definition->title,
                    umi_status_text(status));
            }
        }

        return status;
    }
}

UmiStatus umi_developer_workbench_command_service_create(
    UmiCommandRegistry *commands,
    UmiDeveloperWorkbenchLifecycle *lifecycle,
    UmiDeveloperWorkbenchConfigurationRegistry *configurations,
    const UmiToolchainProfile *profile,
    UmiDeveloperWorkbenchActionRegistry *actions,
    UmiDeveloperWorkbenchCommandService **out_service)
{
    UmiDeveloperWorkbenchCommandService *service;

    if (commands == NULL || lifecycle == NULL ||
        configurations == NULL || profile == NULL ||
        actions == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_service = NULL;
    service = (UmiDeveloperWorkbenchCommandService *)calloc(
        1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    service->commands = commands;
    service->lifecycle = lifecycle;
    service->configurations = configurations;
    service->profile = profile;
    service->actions = actions;

    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_developer_workbench_command_service_destroy(
    UmiDeveloperWorkbenchCommandService *service)
{
    free(service);
}

UmiStatus umi_developer_workbench_command_service_register_all(
    UmiDeveloperWorkbenchCommandService *service)
{
    size_t index;
    const size_t count =
        umi_developer_workbench_command_catalogue_count();

    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DEVELOPER_WORKBENCH_MAX_COMMANDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < count; ++index) {
        const UmiDeveloperWorkbenchCommandDefinition *definition =
            umi_developer_workbench_command_catalogue_at(index);
        UmiCommandDescriptor descriptor;
        uint32_t flags = UMI_COMMAND_NONE;
        UmiStatus status;

        if (definition == NULL) return UMI_STATUS_INTERNAL_ERROR;

        service->bindings[index].service = service;
        service->bindings[index].definition = definition;

        if ((definition->flags &
             UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE) != 0U) {
            flags |= UMI_COMMAND_MUTATES_STATE;
        }
        if ((definition->flags &
             UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND) != 0U) {
            flags |= UMI_COMMAND_BACKGROUND;
        }
        if ((definition->flags &
             UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED) != 0U) {
            flags |= UMI_COMMAND_AUDITED;
        }
        if ((definition->flags &
             UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST) != 0U) {
            flags |= UMI_COMMAND_REQUIRES_TRUST;
        }

        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.command_id = definition->command_id;
        descriptor.title = definition->title;
        descriptor.category = definition->category;
        descriptor.description = definition->description;
        descriptor.required_permission = "";
        descriptor.flags = flags;
        descriptor.handler = binding_execute;
        descriptor.enabled = binding_enabled;
        descriptor.user_data = &service->bindings[index];

        status = umi_command_registry_register(
            service->commands, &descriptor);
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_ALREADY_EXISTS) {
            return status;
        }
    }

    service->binding_count = count;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_command_service_set_workspace_trust(
    UmiDeveloperWorkbenchCommandService *service,
    const UmiWorkspaceTrustStore *trust_store)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->trust_store = trust_store;
    return UMI_STATUS_OK;
}

int umi_developer_workbench_command_service_is_enabled(
    UmiDeveloperWorkbenchCommandService *service,
    const char *command_id,
    const char *argument)
{
    if (service == NULL || command_id == NULL) return 0;

    return umi_command_registry_is_enabled(
        service->commands,
        command_id,
        argument);
}

UmiStatus umi_developer_workbench_command_service_execute(
    UmiDeveloperWorkbenchCommandService *service,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    if (service == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_command_registry_execute(
        service->commands,
        command_id,
        argument,
        out_message,
        message_capacity);
}
