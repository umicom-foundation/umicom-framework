/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/platform.c
 *
 * PURPOSE:
 *   Implement the Framework IDE integration composition root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/platform.h"

#include <stdlib.h>
#include <string.h>

struct UmiIdeIntegrationPlatform {
    UmiIdeIntegrationBindings bindings;
    UmiIdeCrossNavigation navigation;
    UmiIdeWorkflowService workflow;
    UmiIdeSurfaceRegistry *surfaces;
    UmiIdeInlineController inline_controller;
    char workspace_root[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    uint64_t revision;
    int navigation_ready;
    int workflow_ready;
    int inline_ready;
};

UmiStatus umi_ide_integration_platform_create(
    const char *workspace_root,
    const UmiIdeIntegrationBindings *bindings,
    UmiIdeIntegrationPlatform **out_platform)
{
    UmiIdeIntegrationPlatform *platform;
    size_t root_length;
    UmiStatus status;

    if (workspace_root == NULL || workspace_root[0] == '\0' ||
        bindings == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    root_length = strlen(workspace_root);
    if (root_length >= UMI_IDE_INTEGRATION_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_platform = NULL;

    platform = (UmiIdeIntegrationPlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    platform->bindings = *bindings;
    (void)memcpy(
        platform->workspace_root,
        workspace_root,
        root_length + 1U);
    platform->revision = 1U;

    status = umi_ide_cross_navigation_init(
        &platform->navigation,
        &platform->bindings);
    if (status == UMI_STATUS_OK) platform->navigation_ready = 1;

    if (status == UMI_STATUS_OK) {
        status = umi_ide_workflow_service_init(
            &platform->workflow,
            &platform->bindings,
            workspace_root);
    }
    if (status == UMI_STATUS_OK) platform->workflow_ready = 1;

    if (status == UMI_STATUS_OK) {
        status = umi_ide_surface_registry_create(&platform->surfaces);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ide_builtin_surfaces_install(platform->surfaces);
    }

    if (status != UMI_STATUS_OK) {
        umi_ide_integration_platform_destroy(platform);
        return status;
    }

    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_ide_integration_platform_destroy(
    UmiIdeIntegrationPlatform *platform)
{
    if (platform == NULL) return;

    if (platform->inline_ready) {
        umi_ide_inline_controller_deinit(&platform->inline_controller);
    }
    if (platform->navigation_ready) {
        umi_ide_cross_navigation_deinit(&platform->navigation);
    }

    umi_ide_surface_registry_destroy(platform->surfaces);
    free(platform);
}

UmiStatus umi_ide_integration_platform_enable_inline_ai(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits)
{
    UmiStatus status;

    if (platform == NULL || executor == NULL || edits == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (platform->inline_ready) {
        umi_ide_inline_controller_deinit(&platform->inline_controller);
        platform->inline_ready = 0;
    }

    status = umi_ide_inline_controller_init(
        &platform->inline_controller,
        executor,
        edits);

    if (status == UMI_STATUS_OK) {
        platform->inline_ready = 1;
        platform->revision += 1U;
    }

    return status;
}

UmiStatus umi_ide_integration_platform_set_debug_resolver(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data)
{
    if (platform == NULL || !platform->navigation_ready) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_ide_cross_navigation_set_debug_resolver(
        &platform->navigation,
        resolver,
        user_data);
}

UmiStatus umi_ide_integration_platform_set_workflow_policy(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeWorkflowPolicy *policy)
{
    UmiStatus status;

    if (platform == NULL || policy == NULL || !platform->workflow_ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_workflow_service_set_policy(
        &platform->workflow,
        policy);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ide_integration_platform_refresh(
    UmiIdeIntegrationPlatform *platform)
{
    UmiStatus status;

    if (platform == NULL || !platform->workflow_ready) {
        return UMI_STATUS_INVALID_STATE;
    }

    if (platform->bindings.ai_developer != NULL) {
        status = umi_ai_developer_experience_platform_refresh(
            platform->bindings.ai_developer);
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_ide_workflow_service_refresh(&platform->workflow);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_ide_integration_platform_snapshot(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeIntegrationPlatformSnapshot *out_snapshot)
{
    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    out_snapshot->context = platform->workflow.context;
    out_snapshot->workflow = platform->workflow.report;
    out_snapshot->surface_count =
        umi_ide_surface_registry_count(platform->surfaces);
    out_snapshot->navigation_history_count =
        umi_ide_navigation_history_count(platform->navigation.history);
    out_snapshot->inline_ready = platform->inline_ready;
    out_snapshot->revision = platform->revision;

    if (platform->inline_ready) {
        (void)umi_ide_inline_controller_snapshot(
            &platform->inline_controller,
            &out_snapshot->inline_suggestion);
    }

    return UMI_STATUS_OK;
}

UmiIdeIntegrationBindings *umi_ide_integration_platform_bindings(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL ? &platform->bindings : NULL;
}

UmiIdeCrossNavigation *umi_ide_integration_platform_navigation(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL && platform->navigation_ready
        ? &platform->navigation
        : NULL;
}

UmiIdeInlineController *umi_ide_integration_platform_inline(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL && platform->inline_ready
        ? &platform->inline_controller
        : NULL;
}

UmiIdeWorkflowService *umi_ide_integration_platform_workflow(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL && platform->workflow_ready
        ? &platform->workflow
        : NULL;
}

UmiIdeSurfaceRegistry *umi_ide_integration_platform_surfaces(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL ? platform->surfaces : NULL;
}

const char *umi_ide_integration_platform_workspace_root(
    const UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL ? platform->workspace_root : NULL;
}
