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

/*
 * Initialise ide integration platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ide_integration_platform_create(
    const char *workspace_root,
    const UmiIdeIntegrationBindings *bindings,
    UmiIdeIntegrationPlatform **out_platform)
{
    UmiIdeIntegrationPlatform *platform;
    size_t root_length;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace_root == NULL || workspace_root[0] == '\0' ||
        bindings == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    root_length = strlen(workspace_root);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (root_length >= UMI_IDE_INTEGRATION_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_platform = NULL;

    platform = (UmiIdeIntegrationPlatform *)calloc(1U, sizeof(*platform));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->navigation_ready = 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ide_workflow_service_init(
            &platform->workflow,
            &platform->bindings,
            workspace_root);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->workflow_ready = 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ide_surface_registry_create(&platform->surfaces);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ide_builtin_surfaces_install(platform->surfaces);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ide_integration_platform_destroy(platform);
        return status;
    }

    *out_platform = platform;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ide integration platform so the same storage can be
 * reused safely.
 */
void umi_ide_integration_platform_destroy(
    UmiIdeIntegrationPlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return;

    /* Apply this operation only while the related capability or state is available. */
    if (platform->inline_ready) {
        umi_ide_inline_controller_deinit(&platform->inline_controller);
    }
    /* Apply this operation only while the related capability or state is available. */
    if (platform->navigation_ready) {
        umi_ide_cross_navigation_deinit(&platform->navigation);
    }

    umi_ide_surface_registry_destroy(platform->surfaces);
    free(platform);
}

/*
 * Provide the ide integration platform enable inline ai operation used by this module and
 * its client applications.
 */
UmiStatus umi_ide_integration_platform_enable_inline_ai(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || executor == NULL || edits == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (platform->inline_ready) {
        umi_ide_inline_controller_deinit(&platform->inline_controller);
        platform->inline_ready = 0;
    }

    status = umi_ide_inline_controller_init(
        &platform->inline_controller,
        executor,
        edits);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        platform->inline_ready = 1;
        platform->revision += 1U;
    }

    return status;
}

/*
 * Provide the ide integration platform set debug resolver operation used by this module
 * and its client applications.
 */
UmiStatus umi_ide_integration_platform_set_debug_resolver(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || !platform->navigation_ready) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_ide_cross_navigation_set_debug_resolver(
        &platform->navigation,
        resolver,
        user_data);
}

/*
 * Provide the ide integration platform set workflow policy operation used by this module
 * and its client applications.
 */
UmiStatus umi_ide_integration_platform_set_workflow_policy(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeWorkflowPolicy *policy)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || policy == NULL || !platform->workflow_ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_workflow_service_set_policy(
        &platform->workflow,
        policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ide integration platform refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_integration_platform_refresh(
    UmiIdeIntegrationPlatform *platform)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || !platform->workflow_ready) {
        return UMI_STATUS_INVALID_STATE;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->bindings.ai_developer != NULL) {
        status = umi_ai_developer_experience_platform_refresh(
            platform->bindings.ai_developer);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_ide_workflow_service_refresh(&platform->workflow);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the ide integration platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_integration_platform_snapshot(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeIntegrationPlatformSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Apply this operation only while the related capability or state is available. */
    if (platform->inline_ready) {
        (void)umi_ide_inline_controller_snapshot(
            &platform->inline_controller,
            &out_snapshot->inline_suggestion);
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ide integration platform bindings operation used by this module and its
 * client applications.
 */
UmiIdeIntegrationBindings *umi_ide_integration_platform_bindings(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL ? &platform->bindings : NULL;
}

/*
 * Provide the ide integration platform navigation operation used by this module and its
 * client applications.
 */
UmiIdeCrossNavigation *umi_ide_integration_platform_navigation(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL && platform->navigation_ready
        ? &platform->navigation
        : NULL;
}

/*
 * Provide the ide integration platform inline operation used by this module and its client
 * applications.
 */
UmiIdeInlineController *umi_ide_integration_platform_inline(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL && platform->inline_ready
        ? &platform->inline_controller
        : NULL;
}

/*
 * Provide the ide integration platform workflow operation used by this module and its
 * client applications.
 */
UmiIdeWorkflowService *umi_ide_integration_platform_workflow(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL && platform->workflow_ready
        ? &platform->workflow
        : NULL;
}

/*
 * Provide the ide integration platform surfaces operation used by this module and its
 * client applications.
 */
UmiIdeSurfaceRegistry *umi_ide_integration_platform_surfaces(
    UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL ? platform->surfaces : NULL;
}

/*
 * Provide the ide integration platform workspace root operation used by this module and
 * its client applications.
 */
const char *umi_ide_integration_platform_workspace_root(
    const UmiIdeIntegrationPlatform *platform)
{
    return platform != NULL ? platform->workspace_root : NULL;
}
