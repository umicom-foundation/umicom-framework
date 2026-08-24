/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/platform.h
 *
 * PURPOSE:
 *   Compose the final Framework-owned runtime needed by a thin Umicom Studio
 *   host: Application Shell, IDE Integration, commands, aliases, surface
 *   activation, document tabs/title, selections, status/badges and layouts.
 *
 * APPLICATION BOUNDARY:
 *   The application is expected to create toolkit widgets, render Framework view
 *   models and forward user intent. Core IDE behavior remains here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_PLATFORM_H
#define UMICOM_STUDIO_RUNTIME_PLATFORM_H

#include "umicom/ai_developer_experience/command_registry_bridge.h"
#include "umicom/ide_integration/command_registry_bridge.h"
#include "umicom/studio_runtime/ai_context_sync.h"
#include "umicom/studio_runtime/badge_sync.h"
#include "umicom/studio_runtime/command_alias_registry.h"
#include "umicom/studio_runtime/command_execution.h"
#include "umicom/studio_runtime/command_search.h"
#include "umicom/studio_runtime/command_state_sync.h"
#include "umicom/studio_runtime/document_sync.h"
#include "umicom/studio_runtime/document_tabs.h"
#include "umicom/studio_runtime/layout_catalogue.h"
#include "umicom/studio_runtime/status_sync.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeSnapshot {
    UmiIdeIntegrationPlatformSnapshot ide;
    UmiStudioRuntimeDocumentSync document_sync;
    UmiStudioRuntimeDocumentTabs tabs;
    UmiStudioRuntimeStatusModel status;
    UmiStudioRuntimeSelection selection;
    UmiStudioRuntimeCommandSyncReport command_sync;
    char active_layout_preset_id[UMI_STUDIO_RUNTIME_ID_CAPACITY];
    char window_title[768];
    size_t registered_alias_count;
    uint64_t revision;
} UmiStudioRuntimeSnapshot;

typedef struct UmiStudioRuntimePlatform UmiStudioRuntimePlatform;

UmiStatus umi_studio_runtime_platform_create(
    const UmiStudioRuntimeBindings *bindings,
    const char *workspace_name,
    const char *actor_id,
    UmiStudioRuntimePlatform **out_platform);

void umi_studio_runtime_platform_destroy(
    UmiStudioRuntimePlatform *platform);

UmiStatus umi_studio_runtime_platform_refresh(
    UmiStudioRuntimePlatform *platform);

UmiStatus umi_studio_runtime_platform_set_actor(
    UmiStudioRuntimePlatform *platform,
    const char *actor_id);

UmiStatus umi_studio_runtime_platform_select_layout(
    UmiStudioRuntimePlatform *platform,
    const char *preset_id);

UmiStatus umi_studio_runtime_platform_set_selection(
    UmiStudioRuntimePlatform *platform,
    const UmiStudioRuntimeSelectionRouter *selection);

UmiStatus umi_studio_runtime_platform_execute_command(
    UmiStudioRuntimePlatform *platform,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

UmiStatus umi_studio_runtime_platform_execute_contribution(
    UmiStudioRuntimePlatform *platform,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

UmiStatus umi_studio_runtime_platform_snapshot(
    UmiStudioRuntimePlatform *platform,
    UmiStudioRuntimeSnapshot *out_snapshot);

UmiStudioRuntimeBindings *umi_studio_runtime_platform_bindings(
    UmiStudioRuntimePlatform *platform);

UmiStudioRuntimeSelectionRouter *umi_studio_runtime_platform_selection(
    UmiStudioRuntimePlatform *platform);

const UmiStudioRuntimeStatusModel *umi_studio_runtime_platform_status(
    const UmiStudioRuntimePlatform *platform);

const UmiStudioRuntimeDocumentTabs *umi_studio_runtime_platform_tabs(
    const UmiStudioRuntimePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
