/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/command_service.c
 *
 * PURPOSE:
 *   Implement executable/disable-aware professional workbench commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Provide the workspace is trusted operation used by this module and its client
 * applications.
 */
static int workspace_is_trusted(
    UmiDeveloperWorkbenchCommandService *service)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiWorkspaceTrustRecord trust;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL ||
        service->trust_store == NULL ||
        service->configurations == NULL) {
        return 0;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (umi_developer_workbench_configuration_registry_active(
            service->configurations,
            &configuration) != UMI_STATUS_OK) {
        return 0;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workspace_trust_store_get(
            service->trust_store,
            configuration.source_root,
            &trust) != UMI_STATUS_OK) {
        return 0;
    }

    return trust.level == UMI_WORKSPACE_TRUSTED;
}

/* Provide the lifecycle enabled operation used by this module and its client applications. */
static int lifecycle_enabled(
    UmiDeveloperWorkbenchCommandService *service,
    UmiDeveloperWorkbenchCommandAction action)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiDeveloperWorkbenchReadiness readiness;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL ||
        service->configurations == NULL ||
        service->profile == NULL) {
        return 0;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL) {
        return service->lifecycle != NULL &&
            umi_developer_workbench_lifecycle_is_running(
                service->lifecycle);
    }

    /* Apply this operation only while the related capability or state is available. */
    if (umi_developer_workbench_configuration_registry_active(
            service->configurations,
            &configuration) != UMI_STATUS_OK) {
        return 0;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_developer_workbench_readiness_evaluate(
            &configuration,
            service->profile,
            &readiness) != UMI_STATUS_OK) {
        return 0;
    }

    /* Select the behaviour associated with the requested command or state value. */
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

/* Provide the binding enabled operation used by this module and its client applications. */
static int binding_enabled(void *user_data, const char *argument)
{
    CommandBinding *binding = (CommandBinding *)user_data;
    UmiDeveloperWorkbenchCommandService *service;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->service == NULL ||
        binding->definition == NULL) {
        return 0;
    }

    service = binding->service;

    /* Apply this branch only when its contract condition is satisfied. */
    if ((binding->definition->flags &
         UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST) != 0U &&
        !workspace_is_trusted(service)) {
        return 0;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (binding->definition->action ==
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL) {
        return umi_developer_workbench_action_registry_is_enabled(
            service->actions,
            binding->definition->command_id,
            argument);
    }

    return lifecycle_enabled(service, binding->definition->action);
}

/*
 * Perform binding through the module contract so client applications do not duplicate its
 * policy.
 */
static UmiStatus binding_execute(void *user_data,
                                 const char *argument,
                                 char *out_message,
                                 size_t message_capacity)
{
    CommandBinding *binding = (CommandBinding *)user_data;
    UmiDeveloperWorkbenchCommandService *service;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->service == NULL ||
        binding->definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    service = binding->service;

    /* Apply this operation only while the related capability or state is available. */
    if (!binding_enabled(binding, argument)) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_message != NULL && message_capacity > 0U) {
            (void)snprintf(
                out_message,
                message_capacity,
                "%s",
                "Command is unavailable in the current context.");
        }
        return UMI_STATUS_UNAVAILABLE;
    }

    /* Apply this branch only when its contract condition is satisfied. */
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

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_message != NULL && message_capacity > 0U) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (report.last_output[0] != '\0') {
                (void)snprintf(out_message,
                               message_capacity,
                               "%s",
                               report.last_output);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
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

/*
 * Initialise developer workbench command service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_command_service_create(
    UmiCommandRegistry *commands,
    UmiDeveloperWorkbenchLifecycle *lifecycle,
    UmiDeveloperWorkbenchConfigurationRegistry *configurations,
    const UmiToolchainProfile *profile,
    UmiDeveloperWorkbenchActionRegistry *actions,
    UmiDeveloperWorkbenchCommandService **out_service)
{
    UmiDeveloperWorkbenchCommandService *service;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (commands == NULL || lifecycle == NULL ||
        configurations == NULL || profile == NULL ||
        actions == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_service = NULL;
    service = (UmiDeveloperWorkbenchCommandService *)calloc(
        1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    service->commands = commands;
    service->lifecycle = lifecycle;
    service->configurations = configurations;
    service->profile = profile;
    service->actions = actions;

    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer workbench command service so the same storage
 * can be reused safely.
 */
void umi_developer_workbench_command_service_destroy(
    UmiDeveloperWorkbenchCommandService *service)
{
    free(service);
}

/*
 * Provide the developer workbench command service register all operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_command_service_register_all(
    UmiDeveloperWorkbenchCommandService *service)
{
    size_t index;
    const size_t count =
        umi_developer_workbench_command_catalogue_count();

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_DEVELOPER_WORKBENCH_MAX_COMMANDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const UmiDeveloperWorkbenchCommandDefinition *definition =
            umi_developer_workbench_command_catalogue_at(index);
        UmiCommandDescriptor descriptor;
        uint32_t flags = UMI_COMMAND_NONE;
        UmiStatus status;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (definition == NULL) return UMI_STATUS_INTERNAL_ERROR;

        service->bindings[index].service = service;
        service->bindings[index].definition = definition;

        /* Apply this branch only when its contract condition is satisfied. */
        if ((definition->flags &
             UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE) != 0U) {
            flags |= UMI_COMMAND_MUTATES_STATE;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((definition->flags &
             UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND) != 0U) {
            flags |= UMI_COMMAND_BACKGROUND;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((definition->flags &
             UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED) != 0U) {
            flags |= UMI_COMMAND_AUDITED;
        }
        /* Apply this branch only when its contract condition is satisfied. */
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
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_ALREADY_EXISTS) {
            return status;
        }
    }

    service->binding_count = count;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer workbench command service set workspace trust operation used by
 * this module and its client applications.
 */
UmiStatus umi_developer_workbench_command_service_set_workspace_trust(
    UmiDeveloperWorkbenchCommandService *service,
    const UmiWorkspaceTrustStore *trust_store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->trust_store = trust_store;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer workbench command service is enabled operation used by this module
 * and its client applications.
 */
int umi_developer_workbench_command_service_is_enabled(
    UmiDeveloperWorkbenchCommandService *service,
    const char *command_id,
    const char *argument)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || command_id == NULL) return 0;

    return umi_command_registry_is_enabled(
        service->commands,
        command_id,
        argument);
}

/*
 * Perform developer workbench command service through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_developer_workbench_command_service_execute(
    UmiDeveloperWorkbenchCommandService *service,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
