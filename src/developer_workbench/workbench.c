/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/workbench.c
 *
 * PURPOSE:
 *   Implement the reusable Framework developer-workbench composition root.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/workbench.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbench {
    UmiCommandRegistry *commands;
    UmiDeveloperWorkbenchActionRegistry *actions;
    UmiDeveloperWorkbenchConfigurationRegistry *configurations;
    UmiDeveloperWorkbenchOperationHistory *history;
    UmiDeveloperWorkbenchLifecycle *lifecycle;
    UmiDeveloperWorkbenchCommandService *command_service;
    UmiDeveloperWorkbenchSearchEngine *search_engine;
    UmiRecentItemRegistry *recent_items;
    UmiDeveloperWorkbenchPerspectiveRegistry *perspectives;
    const UmiToolchainProfile *toolchain;
    UmiEnvironmentPlan *environment;
    UmiFileIndex *file_index;
    const UmiWorkspaceTrustStore *trust_store;
    uint64_t revision;
};

UmiStatus umi_developer_workbench_create(
    const UmiDeveloperWorkbenchBindings *bindings,
    UmiDeveloperWorkbench **out_workbench)
{
    UmiDeveloperWorkbench *workbench;
    UmiDeveloperWorkbenchSearchProvider provider;
    UmiStatus status = UMI_STATUS_OK;

    if (bindings == NULL || bindings->toolchain == NULL ||
        out_workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_workbench = NULL;
    workbench =
        (UmiDeveloperWorkbench *)calloc(1U, sizeof(*workbench));
    if (workbench == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    workbench->toolchain = bindings->toolchain;
    workbench->environment = bindings->environment;
    workbench->file_index = bindings->file_index;
    workbench->trust_store = bindings->trust_store;
    workbench->revision = 1U;

    if (status == UMI_STATUS_OK) {
        status = umi_command_registry_create(&workbench->commands);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_action_registry_create(
            &workbench->actions);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_configuration_registry_create(
            &workbench->configurations);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_operation_history_create(
            &workbench->history);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_lifecycle_create(
            workbench->toolchain,
            workbench->environment,
            workbench->configurations,
            workbench->history,
            &workbench->lifecycle);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_command_service_create(
            workbench->commands,
            workbench->lifecycle,
            workbench->configurations,
            workbench->toolchain,
            workbench->actions,
            &workbench->command_service);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_command_service_set_workspace_trust(
            workbench->command_service,
            workbench->trust_store);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_command_service_register_all(
            workbench->command_service);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_search_engine_create(
            &workbench->search_engine);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_platform_recent_items_registry_create(
            &workbench->recent_items);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_perspective_registry_create(
            &workbench->perspectives);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_workbench_builtin_perspectives_register(
            workbench->perspectives);
    }

    if (status == UMI_STATUS_OK) {
        umi_developer_workbench_command_search_provider_init(
            &provider, workbench->commands);
        status = umi_developer_workbench_search_engine_register(
            workbench->search_engine, &provider);
    }

    if (status == UMI_STATUS_OK && workbench->file_index != NULL) {
        umi_developer_workbench_file_search_provider_init(
            &provider, workbench->file_index);
        status = umi_developer_workbench_search_engine_register(
            workbench->search_engine, &provider);
    }

    if (status == UMI_STATUS_OK) {
        umi_developer_workbench_recent_project_provider_init(
            &provider, workbench->recent_items);
        status = umi_developer_workbench_search_engine_register(
            workbench->search_engine, &provider);
    }

    if (status != UMI_STATUS_OK) {
        umi_developer_workbench_destroy(workbench);
        return status;
    }

    *out_workbench = workbench;
    return UMI_STATUS_OK;
}

void umi_developer_workbench_destroy(
    UmiDeveloperWorkbench *workbench)
{
    if (workbench == NULL) return;

    umi_developer_workbench_perspective_registry_destroy(
        workbench->perspectives);
    umi_platform_recent_items_registry_destroy(workbench->recent_items);
    umi_developer_workbench_search_engine_destroy(workbench->search_engine);
    umi_developer_workbench_command_service_destroy(
        workbench->command_service);
    umi_developer_workbench_lifecycle_destroy(workbench->lifecycle);
    umi_developer_workbench_operation_history_destroy(workbench->history);
    umi_developer_workbench_configuration_registry_destroy(
        workbench->configurations);
    umi_developer_workbench_action_registry_destroy(workbench->actions);
    umi_command_registry_destroy(workbench->commands);
    free(workbench);
}

UmiStatus umi_developer_workbench_add_configuration(
    UmiDeveloperWorkbench *workbench,
    const UmiDeveloperWorkbenchConfiguration *configuration,
    int make_active)
{
    UmiStatus status;

    if (workbench == NULL || configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_configuration_registry_upsert(
        workbench->configurations,
        configuration);
    if (status == UMI_STATUS_OK && make_active) {
        status = umi_developer_workbench_configuration_registry_activate(
            workbench->configurations,
            configuration->configuration_id);
    }

    if (status == UMI_STATUS_OK) workbench->revision += 1U;
    return status;
}

UmiStatus umi_developer_workbench_bind_action(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    UmiDeveloperWorkbenchActionHandler handler,
    UmiDeveloperWorkbenchActionEnabled enabled,
    void *user_data)
{
    UmiStatus status;

    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_workbench_action_registry_bind(
        workbench->actions,
        command_id,
        handler,
        enabled,
        user_data);
    if (status == UMI_STATUS_OK) workbench->revision += 1U;
    return status;
}

UmiStatus umi_developer_workbench_execute_command(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    return umi_developer_workbench_command_service_execute(
        workbench->command_service,
        command_id,
        argument,
        out_message,
        message_capacity);
}

int umi_developer_workbench_command_enabled(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    const char *argument)
{
    return workbench != NULL &&
        umi_developer_workbench_command_service_is_enabled(
            workbench->command_service,
            command_id,
            argument);
}

UmiStatus umi_developer_workbench_search(
    UmiDeveloperWorkbench *workbench,
    const char *query,
    UmiDeveloperWorkbenchSearchSession *session)
{
    if (workbench == NULL || session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_developer_workbench_search_session_query(
        session,
        workbench->search_engine,
        query);
}

UmiStatus umi_developer_workbench_activate_perspective(
    UmiDeveloperWorkbench *workbench,
    const char *perspective_id)
{
    UmiStatus status;

    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_workbench_perspective_registry_activate(
        workbench->perspectives,
        perspective_id);
    if (status == UMI_STATUS_OK) workbench->revision += 1U;
    return status;
}

UmiStatus umi_developer_workbench_snapshot(
    UmiDeveloperWorkbench *workbench,
    UmiDeveloperWorkbenchSnapshot *out_snapshot)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *active;

    if (workbench == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->command_count =
        umi_command_registry_count(workbench->commands);
    out_snapshot->external_action_count =
        umi_developer_workbench_action_registry_count(workbench->actions);
    out_snapshot->configuration_count =
        umi_developer_workbench_configuration_registry_count(
            workbench->configurations);
    out_snapshot->search_provider_count =
        umi_developer_workbench_search_engine_provider_count(
            workbench->search_engine);
    out_snapshot->recent_project_count =
        umi_platform_recent_items_registry_count(workbench->recent_items);
    out_snapshot->perspective_count =
        umi_developer_workbench_perspective_registry_count(
            workbench->perspectives);
    out_snapshot->operation_history_count =
        umi_developer_workbench_operation_history_count(workbench->history);

    active = umi_developer_workbench_perspective_registry_active(
        workbench->perspectives);
    if (active != NULL) {
        (void)snprintf(out_snapshot->active_perspective_id,
                       sizeof(out_snapshot->active_perspective_id),
                       "%s",
                       active->perspective_id);
    }

    out_snapshot->revision = workbench->revision;
    return UMI_STATUS_OK;
}

UmiCommandRegistry *umi_developer_workbench_commands(
    UmiDeveloperWorkbench *workbench)
{
    return workbench != NULL ? workbench->commands : NULL;
}

UmiDeveloperWorkbenchConfigurationRegistry *
umi_developer_workbench_configurations(
    UmiDeveloperWorkbench *workbench)
{
    return workbench != NULL ? workbench->configurations : NULL;
}

UmiRecentItemRegistry *umi_developer_workbench_recent_items(
    UmiDeveloperWorkbench *workbench)
{
    return workbench != NULL ? workbench->recent_items : NULL;
}

UmiDeveloperWorkbenchPerspectiveRegistry *
umi_developer_workbench_perspectives(
    UmiDeveloperWorkbench *workbench)
{
    return workbench != NULL ? workbench->perspectives : NULL;
}

UmiDeveloperWorkbenchOperationHistory *
umi_developer_workbench_history(
    UmiDeveloperWorkbench *workbench)
{
    return workbench != NULL ? workbench->history : NULL;
}
