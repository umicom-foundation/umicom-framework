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

UmiStatus umi_developer_productivity_platform_create(
    const UmiDeveloperProductivityPlatformBindings *bindings,
    UmiDeveloperProductivityPlatform **out_platform)
{
    UmiDeveloperProductivityPlatform *platform;
    UmiDeveloperWorkbench *workbench;
    UmiStatus status;

    if (bindings == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_platform = NULL;
    platform = (UmiDeveloperProductivityPlatform *)calloc(
        1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    umi_developer_output_channels_init(&platform->output);
    platform->revision = 1U;

    status = umi_developer_project_workbench_platform_create(
        &bindings->workbench,
        &platform->project_workbench);

    if (status == UMI_STATUS_OK) {
        status = umi_developer_diagnostic_pipeline_create(
            &platform->diagnostics);
    }

    if (status == UMI_STATUS_OK) {
        status = umi_developer_navigation_service_create(
            &platform->navigation);
    }

    if (status == UMI_STATUS_OK && bindings->clock != NULL) {
        status = umi_developer_terminal_workspace_create(
            bindings->clock,
            &platform->terminal);
    }

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

    if (status == UMI_STATUS_OK) {
        (void)umi_developer_productivity_workbench_bridge_set_terminal(
            platform->bridge,
            platform->terminal);
        status = umi_developer_productivity_workbench_bridge_bind(
            platform->bridge);
    }

    if (status != UMI_STATUS_OK) {
        umi_developer_productivity_platform_destroy(platform);
        return status;
    }

    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_developer_productivity_platform_destroy(
    UmiDeveloperProductivityPlatform *platform)
{
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

UmiStatus umi_developer_productivity_platform_open_git(
    UmiDeveloperProductivityPlatform *platform,
    const char *root)
{
    UmiDeveloperSourceControl *controller = NULL;
    UmiStatus status;

    if (platform == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_source_control_create_git(root, &controller);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_source_control_refresh(controller);
    if (status != UMI_STATUS_OK) {
        umi_developer_source_control_destroy(controller);
        return status;
    }

    umi_developer_source_control_destroy(platform->source_control);
    platform->source_control = controller;

    status = umi_developer_productivity_workbench_bridge_set_source_control(
        platform->bridge,
        platform->source_control);
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

void umi_developer_productivity_platform_close_source_control(
    UmiDeveloperProductivityPlatform *platform)
{
    if (platform == NULL) return;

    umi_developer_source_control_destroy(platform->source_control);
    platform->source_control = NULL;
    (void)umi_developer_productivity_workbench_bridge_set_source_control(
        platform->bridge, NULL);
    platform->revision += 1U;
}

UmiStatus umi_developer_productivity_platform_ingest_output(
    UmiDeveloperProductivityPlatform *platform,
    const char *channel_id,
    const char *channel_title,
    const char *text,
    size_t *out_problems_added)
{
    UmiStatus status;

    if (platform == NULL || channel_id == NULL ||
        channel_title == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_output_channel_append(
        &platform->output,
        channel_id,
        channel_title,
        text);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_diagnostic_pipeline_ingest_text(
        platform->diagnostics,
        text,
        out_problems_added);

    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

UmiStatus umi_developer_productivity_platform_snapshot(
    UmiDeveloperProductivityPlatform *platform,
    UmiDeveloperProductivityPlatformSnapshot *out_snapshot)
{
    UmiStatus status;

    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_developer_project_workbench_platform_snapshot(
        platform->project_workbench,
        &out_snapshot->project_workbench);
    if (status != UMI_STATUS_OK) return status;

    if (platform->source_control != NULL) {
        status = umi_developer_source_control_snapshot(
            platform->source_control,
            &out_snapshot->source_control);
        if (status != UMI_STATUS_OK) return status;
        out_snapshot->source_control_open = 1;
    }

    if (platform->terminal != NULL) {
        status = umi_developer_terminal_workspace_snapshot(
            platform->terminal,
            &out_snapshot->terminal);
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

UmiDeveloperProjectWorkbenchPlatform *
umi_developer_productivity_platform_project_workbench(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->project_workbench : NULL;
}

UmiDeveloperDiagnosticPipeline *
umi_developer_productivity_platform_diagnostics(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->diagnostics : NULL;
}

UmiDeveloperNavigationService *
umi_developer_productivity_platform_navigation(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->navigation : NULL;
}

UmiDeveloperOutputChannels *
umi_developer_productivity_platform_output(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? &platform->output : NULL;
}

UmiDeveloperSourceControl *
umi_developer_productivity_platform_source_control(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->source_control : NULL;
}

UmiDeveloperTerminalWorkspace *
umi_developer_productivity_platform_terminal(
    UmiDeveloperProductivityPlatform *platform)
{
    return platform != NULL ? platform->terminal : NULL;
}
