/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/platform.c
 *
 * PURPOSE:
 *   Implement the Framework productivity composition root used by a thin IDE.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/platform.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProductivityPlatform {
    UmiDeveloperProjectWorkbenchPlatform *project_workbench;
    UmiDeveloperSourceControl *source_control;
    UmiDeveloperTerminalWorkspace *terminal;
    UmiDeveloperDiagnosticPipeline *diagnostics;
    UmiDeveloperNavigationService *navigation;
    UmiDeveloperOutputChannels output;
    UmiDeveloperProductivityWorkbenchBridge *bridge;
    uint64_t revision;
};

/*
 * Initialise developer productivity platform from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_productivity_platform_create(
    const UmiDeveloperProductivityPlatformBindings *bindings,
    UmiDeveloperProductivityPlatform **out_platform)
{
    UmiDeveloperProductivityPlatform *platform;
    UmiDeveloperWorkbench *workbench;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_platform = NULL;
    platform = (UmiDeveloperProductivityPlatform *)calloc(
        1U, sizeof(*platform));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    umi_developer_output_channels_init(&platform->output);
    platform->revision = 1U;

    status = umi_developer_project_workbench_platform_create(
        &bindings->workbench,
        &platform->project_workbench);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_developer_diagnostic_pipeline_create(
            &platform->diagnostics);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_developer_navigation_service_create(
            &platform->navigation);
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && bindings->clock != NULL) {
        status = umi_developer_terminal_workspace_create(
            bindings->clock,
            &platform->terminal);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workbench = umi_developer_project_workbench_platform_workbench(
            platform->project_workbench);

        status = umi_developer_productivity_workbench_bridge_create(
            workbench,
            platform->diagnostics,
            platform->navigation,
            &platform->output,
            &platform->bridge);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_developer_productivity_workbench_bridge_set_terminal(
            platform->bridge,
            platform->terminal);
        status = umi_developer_productivity_workbench_bridge_bind(
            platform->bridge);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_developer_productivity_platform_destroy(platform);
        return status;
    }

    *out_platform = platform;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer productivity platform so the same storage can
 * be reused safely.
 */
void umi_developer_productivity_platform_destroy(
    UmiDeveloperProductivityPlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return;

    umi_developer_productivity_workbench_bridge_destroy(platform->bridge);
    umi_developer_source_control_destroy(platform->source_control);
    umi_developer_terminal_workspace_destroy(platform->terminal);
    umi_developer_navigation_service_destroy(platform->navigation);
    umi_developer_diagnostic_pipeline_destroy(platform->diagnostics);
    umi_developer_project_workbench_platform_destroy(
        platform->project_workbench);
    free(platform);
}

/*
 * Provide the developer productivity platform open git operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_productivity_platform_open_git(
    UmiDeveloperProductivityPlatform *platform,
    const char *root)
{
    UmiDeveloperSourceControl *controller = NULL;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_source_control_create_git(root, &controller);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_source_control_refresh(controller);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_developer_source_control_destroy(controller);
        return status;
    }

    umi_developer_source_control_destroy(platform->source_control);
    platform->source_control = controller;

    status = umi_developer_productivity_workbench_bridge_set_source_control(
        platform->bridge,
        platform->source_control);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the developer productivity platform close source control operation used by this
 * module and its client applications.
 */
void umi_developer_productivity_platform_close_source_control(
    UmiDeveloperProductivityPlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return;

    umi_developer_source_control_destroy(platform->source_control);
    platform->source_control = NULL;
    (void)umi_developer_productivity_workbench_bridge_set_source_control(
        platform->bridge, NULL);
    platform->revision += 1U;
}

/*
 * Provide the developer productivity platform ingest output operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_productivity_platform_ingest_output(
    UmiDeveloperProductivityPlatform *platform,
    const char *channel_id,
    const char *channel_title,
    const char *text,
    size_t *out_problems_added)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || channel_id == NULL ||
        channel_title == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_output_channel_append(
        &platform->output,
        channel_id,
        channel_title,
        text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_diagnostic_pipeline_ingest_text(
        platform->diagnostics,
        text,
        out_problems_added);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the developer productivity platform snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_productivity_platform_snapshot(
    UmiDeveloperProductivityPlatform *platform,
    UmiDeveloperProductivityPlatformSnapshot *out_snapshot)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_developer_project_workbench_platform_snapshot(
        platform->project_workbench,
        &out_snapshot->project_workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->source_control != NULL) {
        status = umi_developer_source_control_snapshot(
            platform->source_control,
            &out_snapshot->source_control);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        out_snapshot->source_control_open = 1;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->terminal != NULL) {
        status = umi_developer_terminal_workspace_snapshot(
            platform->terminal,
            &out_snapshot->terminal);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        out_snapshot->terminal_available = 1;
    }

    out_snapshot->problems =
        umi_developer_problem_store_snapshot(
            umi_developer_diagnostic_pipeline_problems(
                platform->diagnostics));
    out_snapshot->output_channel_count = platform->output.count;
    out_snapshot->symbol_count =
        umi_developer_symbol_index_count(platform->navigation->symbols);
    out_snapshot->revision = platform->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer productivity platform project workbench operation used by this
 * module and its client applications.
 */
UmiDeveloperProjectWorkbenchPlatform *
umi_developer_productivity_platform_project_workbench(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->project_workbench : NULL;
}

/*
 * Provide the developer productivity platform diagnostics operation used by this module
 * and its client applications.
 */
UmiDeveloperDiagnosticPipeline *
umi_developer_productivity_platform_diagnostics(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->diagnostics : NULL;
}

/*
 * Provide the developer productivity platform navigation operation used by this module and
 * its client applications.
 */
UmiDeveloperNavigationService *
umi_developer_productivity_platform_navigation(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->navigation : NULL;
}

/*
 * Provide the developer productivity platform output operation used by this module and its
 * client applications.
 */
UmiDeveloperOutputChannels *
umi_developer_productivity_platform_output(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? &platform->output : NULL;
}

/*
 * Provide the developer productivity platform source control operation used by this module
 * and its client applications.
 */
UmiDeveloperSourceControl *
umi_developer_productivity_platform_source_control(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->source_control : NULL;
}

/*
 * Provide the developer productivity platform terminal operation used by this module and
 * its client applications.
 */
UmiDeveloperTerminalWorkspace *
umi_developer_productivity_platform_terminal(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->terminal : NULL;
}
