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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the studio runtime snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the studio runtime platform data shared with callers of this public contract.
 */
typedef struct UmiStudioRuntimePlatform UmiStudioRuntimePlatform;

/**
 * Initialise studio runtime platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_runtime_platform_create(
    const UmiStudioRuntimeBindings *bindings,
    const char *workspace_name,
    const char *actor_id,
    UmiStudioRuntimePlatform **out_platform);

/**
 * Release or reset state held by studio runtime platform so the same storage can be reused
 * safely.
 */
void umi_studio_runtime_platform_destroy(
    UmiStudioRuntimePlatform *platform);

/**
 * Provide the studio runtime platform refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_platform_refresh(
    UmiStudioRuntimePlatform *platform);

/**
 * Provide the studio runtime platform set actor operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_runtime_platform_set_actor(
    UmiStudioRuntimePlatform *platform,
    const char *actor_id);

/**
 * Provide the studio runtime platform select layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_runtime_platform_select_layout(
    UmiStudioRuntimePlatform *platform,
    const char *preset_id);

/**
 * Provide the studio runtime platform set selection operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_runtime_platform_set_selection(
    UmiStudioRuntimePlatform *platform,
    const UmiStudioRuntimeSelectionRouter *selection);

/**
 * Provide the studio runtime platform execute command operation used by this module and
 * its client applications.
 */
UmiStatus umi_studio_runtime_platform_execute_command(
    UmiStudioRuntimePlatform *platform,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

/**
 * Provide the studio runtime platform execute contribution operation used by this module
 * and its client applications.
 */
UmiStatus umi_studio_runtime_platform_execute_contribution(
    UmiStudioRuntimePlatform *platform,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

/**
 * Provide the studio runtime platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_runtime_platform_snapshot(
    UmiStudioRuntimePlatform *platform,
    UmiStudioRuntimeSnapshot *out_snapshot);

/**
 * Provide the studio runtime platform bindings operation used by this module and its
 * client applications.
 */
UmiStudioRuntimeBindings *umi_studio_runtime_platform_bindings(
    UmiStudioRuntimePlatform *platform);

/**
 * Provide the studio runtime platform selection operation used by this module and its
 * client applications.
 */
UmiStudioRuntimeSelectionRouter *umi_studio_runtime_platform_selection(
    UmiStudioRuntimePlatform *platform);

/**
 * Provide the studio runtime platform status operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeStatusModel *umi_studio_runtime_platform_status(
    const UmiStudioRuntimePlatform *platform);

/**
 * Provide the studio runtime platform tabs operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeDocumentTabs *umi_studio_runtime_platform_tabs(
    const UmiStudioRuntimePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
