/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/platform.h
 *
 * PURPOSE:
 *   Compose canonical Framework developer services into one reusable IDE
 *   integration platform for thin IDE applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_PLATFORM_H
#define UMICOM_IDE_INTEGRATION_PLATFORM_H

#include "umicom/ide_integration/builtin_surfaces.h"
#include "umicom/ide_integration/cross_navigation.h"
#include "umicom/ide_integration/inline_controller.h"
#include "umicom/ide_integration/workflow_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeIntegrationPlatformSnapshot {
    UmiIdeActiveContext context;
    UmiIdeWorkflowReport workflow;
    UmiIdeInlineSuggestion inline_suggestion;
    size_t surface_count;
    size_t navigation_history_count;
    uint64_t revision;
    int inline_ready;
} UmiIdeIntegrationPlatformSnapshot;

typedef struct UmiIdeIntegrationPlatform UmiIdeIntegrationPlatform;

UmiStatus umi_ide_integration_platform_create(
    const char *workspace_root,
    const UmiIdeIntegrationBindings *bindings,
    UmiIdeIntegrationPlatform **out_platform);

void umi_ide_integration_platform_destroy(
    UmiIdeIntegrationPlatform *platform);

UmiStatus umi_ide_integration_platform_enable_inline_ai(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits);

UmiStatus umi_ide_integration_platform_set_debug_resolver(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data);

UmiStatus umi_ide_integration_platform_set_workflow_policy(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeWorkflowPolicy *policy);

UmiStatus umi_ide_integration_platform_refresh(
    UmiIdeIntegrationPlatform *platform);

UmiStatus umi_ide_integration_platform_snapshot(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeIntegrationPlatformSnapshot *out_snapshot);

UmiIdeIntegrationBindings *umi_ide_integration_platform_bindings(
    UmiIdeIntegrationPlatform *platform);

UmiIdeCrossNavigation *umi_ide_integration_platform_navigation(
    UmiIdeIntegrationPlatform *platform);

UmiIdeInlineController *umi_ide_integration_platform_inline(
    UmiIdeIntegrationPlatform *platform);

UmiIdeWorkflowService *umi_ide_integration_platform_workflow(
    UmiIdeIntegrationPlatform *platform);

UmiIdeSurfaceRegistry *umi_ide_integration_platform_surfaces(
    UmiIdeIntegrationPlatform *platform);

const char *umi_ide_integration_platform_workspace_root(
    const UmiIdeIntegrationPlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
