/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/platform.c
 *
 * PURPOSE:
 *   Implement the final Framework Studio runtime composition root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/platform.h"

#include <stdlib.h>
#include <string.h>

struct UmiStudioRuntimePlatform {
    UmiStudioRuntimeBindings bindings;
    UmiStudioRuntimeCommandAliasRegistry *aliases;
    UmiIdeCommandRegistryBridge *ide_commands;
    UmiAiDeveloperCommandRegistryBridge *ai_commands;
    UmiStudioRuntimeSelectionRouter selection;
    UmiStudioRuntimeDocumentSync document_sync;
    UmiStudioRuntimeDocumentTabs tabs;
    UmiStudioRuntimeStatusModel status;
    UmiStudioRuntimeCommandSyncReport command_sync;
    char workspace_name[256];
    char actor_id[UMI_AI_ID_CAPACITY];
    char active_layout_preset_id[UMI_STUDIO_RUNTIME_ID_CAPACITY];
    uint64_t revision;
};

static UmiStatus sync_command_contexts(UmiStudioRuntimePlatform *platform)
{
    UmiAiDeveloperCommandContext ai_context;
    UmiStatus status;

    status = umi_ide_command_registry_bridge_set_context(
        platform->ide_commands,
        &platform->selection.command_context);
    if (status != UMI_STATUS_OK) return status;

    if (platform->ai_commands != NULL) {
        status = umi_studio_ai_context_sync(
            &platform->selection,
            platform->actor_id,
            &ai_context);
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_command_registry_bridge_set_context(
            platform->ai_commands,
            &ai_context);
    }

    return status;
}

UmiStatus umi_studio_runtime_platform_create(
    const UmiStudioRuntimeBindings *bindings,
    const char *workspace_name,
    const char *actor_id,
    UmiStudioRuntimePlatform **out_platform)
{
    UmiStudioRuntimePlatform *platform;
    UmiIdeIntegrationBindings *ide_bindings;
    const UmiStudioRuntimeLayoutPresetDefinition *default_layout;
    size_t workspace_length;
    size_t actor_length;
    UmiStatus status;

    if (bindings == NULL || workspace_name == NULL ||
        actor_id == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    workspace_length = strlen(workspace_name);
    actor_length = strlen(actor_id);

    if (workspace_length >= 256U ||
        actor_length >= UMI_AI_ID_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_platform = NULL;

    platform = (UmiStudioRuntimePlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    platform->bindings = *bindings;
    (void)memcpy(
        platform->workspace_name,
        workspace_name,
        workspace_length + 1U);
    (void)memcpy(
        platform->actor_id,
        actor_id,
        actor_length + 1U);

    umi_studio_selection_router_init(&platform->selection);
    umi_studio_document_tabs_init(&platform->tabs);
    umi_studio_status_model_init(&platform->status);
    platform->revision = 1U;

    status = umi_studio_document_sync_init(
        &platform->document_sync,
        workspace_name);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_surface_catalogue_install(
            platform->bindings.shell_registry,
            platform->bindings.shell_layout);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ide_command_registry_bridge_create(
            platform->bindings.commands,
            platform->bindings.ide,
            &platform->ide_commands);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ide_command_registry_bridge_register(
            platform->ide_commands);
    }

    ide_bindings = umi_ide_integration_platform_bindings(
        platform->bindings.ide);

    if (status == UMI_STATUS_OK &&
        ide_bindings != NULL &&
        ide_bindings->ai_developer != NULL) {
        status = umi_ai_developer_command_registry_bridge_create(
            platform->bindings.commands,
            ide_bindings->ai_developer,
            &platform->ai_commands);
        if (status == UMI_STATUS_OK) {
            status = umi_ai_developer_command_registry_bridge_register(
                platform->ai_commands);
        }
    }

    if (status == UMI_STATUS_OK) {
        status = umi_studio_command_alias_registry_create(
            &platform->bindings,
            &platform->aliases);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_command_alias_registry_install(
            platform->aliases);
    }

    default_layout = umi_studio_layout_catalogue_find(
        "umicom.studio.layout.default");

    if (status == UMI_STATUS_OK && default_layout != NULL) {
        status = umi_studio_layout_preset_apply(
            &platform->bindings,
            default_layout);

        if (status == UMI_STATUS_OK) {
            (void)strcpy(
                platform->active_layout_preset_id,
                default_layout->preset_id);
        }
    }

    if (status == UMI_STATUS_OK) {
        status = sync_command_contexts(platform);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_runtime_platform_refresh(platform);
    }

    if (status != UMI_STATUS_OK) {
        umi_studio_runtime_platform_destroy(platform);
        return status;
    }

    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_studio_runtime_platform_destroy(
    UmiStudioRuntimePlatform *platform)
{
    if (platform == NULL) return;

    umi_studio_command_alias_registry_destroy(platform->aliases);
    umi_ai_developer_command_registry_bridge_destroy(platform->ai_commands);
    umi_ide_command_registry_bridge_destroy(platform->ide_commands);
    free(platform);
}

UmiStatus umi_studio_runtime_platform_refresh(
    UmiStudioRuntimePlatform *platform)
{
    UmiIdeIntegrationPlatformSnapshot ide_snapshot;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ide_integration_platform_refresh(
        platform->bindings.ide);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_document_sync_refresh(
        &platform->document_sync,
        &platform->bindings);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_document_tabs_refresh(
        &platform->tabs,
        platform->bindings.documents);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_integration_platform_snapshot(
        platform->bindings.ide,
        &ide_snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_status_model_build(
        &platform->status,
        &ide_snapshot,
        &platform->selection);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_status_sync(
        &platform->bindings,
        &platform->status);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_badge_sync(
        &platform->bindings,
        &ide_snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = sync_command_contexts(platform);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_runtime_sync_commands(
        &platform->bindings,
        &platform->command_sync);
    if (status != UMI_STATUS_OK) return status;

    platform->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_platform_set_actor(
    UmiStudioRuntimePlatform *platform,
    const char *actor_id)
{
    size_t length;

    if (platform == NULL || actor_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(actor_id);
    if (length >= sizeof(platform->actor_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(platform->actor_id, actor_id, length + 1U);
    platform->revision += 1U;
    return sync_command_contexts(platform);
}

UmiStatus umi_studio_runtime_platform_select_layout(
    UmiStudioRuntimePlatform *platform,
    const char *preset_id)
{
    const UmiStudioRuntimeLayoutPresetDefinition *preset;
    UmiStatus status;

    if (platform == NULL || preset_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    preset = umi_studio_layout_catalogue_find(preset_id);
    if (preset == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_studio_layout_preset_apply(
        &platform->bindings,
        preset);
    if (status != UMI_STATUS_OK) return status;

    if (strlen(preset_id) >= sizeof(platform->active_layout_preset_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(platform->active_layout_preset_id, preset_id);
    platform->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_platform_set_selection(
    UmiStudioRuntimePlatform *platform,
    const UmiStudioRuntimeSelectionRouter *selection)
{
    if (platform == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    platform->selection = *selection;
    platform->revision += 1U;
    return sync_command_contexts(platform);
}

UmiStatus umi_studio_runtime_platform_execute_command(
    UmiStudioRuntimePlatform *platform,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = sync_command_contexts(platform);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_runtime_execute_command(
        &platform->bindings,
        command_id,
        argument,
        out_message,
        message_capacity);

    if (status == UMI_STATUS_OK) {
        status = umi_studio_runtime_platform_refresh(platform);
    }

    return status;
}

UmiStatus umi_studio_runtime_platform_execute_contribution(
    UmiStudioRuntimePlatform *platform,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = sync_command_contexts(platform);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_runtime_execute_contribution(
        &platform->bindings,
        contribution_id,
        argument,
        out_message,
        message_capacity);

    if (status == UMI_STATUS_OK) {
        status = umi_studio_runtime_platform_refresh(platform);
    }

    return status;
}

UmiStatus umi_studio_runtime_platform_snapshot(
    UmiStudioRuntimePlatform *platform,
    UmiStudioRuntimeSnapshot *out_snapshot)
{
    UmiStatus status;

    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_ide_integration_platform_snapshot(
        platform->bindings.ide,
        &out_snapshot->ide);
    if (status != UMI_STATUS_OK) return status;

    out_snapshot->document_sync = platform->document_sync;
    out_snapshot->tabs = platform->tabs;
    out_snapshot->status = platform->status;
    out_snapshot->selection = platform->selection.state.current;
    out_snapshot->command_sync = platform->command_sync;
    out_snapshot->registered_alias_count =
        umi_studio_command_alias_registry_registered_count(
            platform->aliases);
    out_snapshot->revision = platform->revision;

    (void)strcpy(
        out_snapshot->active_layout_preset_id,
        platform->active_layout_preset_id);
    (void)strcpy(
        out_snapshot->window_title,
        platform->document_sync.window_title.title);

    return UMI_STATUS_OK;
}

UmiStudioRuntimeBindings *umi_studio_runtime_platform_bindings(
    UmiStudioRuntimePlatform *platform)
{
    return platform != NULL ? &platform->bindings : NULL;
}

UmiStudioRuntimeSelectionRouter *umi_studio_runtime_platform_selection(
    UmiStudioRuntimePlatform *platform)
{
    return platform != NULL ? &platform->selection : NULL;
}

const UmiStudioRuntimeStatusModel *umi_studio_runtime_platform_status(
    const UmiStudioRuntimePlatform *platform)
{
    return platform != NULL ? &platform->status : NULL;
}

const UmiStudioRuntimeDocumentTabs *umi_studio_runtime_platform_tabs(
    const UmiStudioRuntimePlatform *platform)
{
    return platform != NULL ? &platform->tabs : NULL;
}
